// Hafta 5 - Haftalik Odev: Renk uzayi gorevlerinin CUDA ile gerceklestirilmesi
// Tum gorevler GPU uzerinde yeniden gerceklestirilmistir:
// 1-2) Her kanal icin histogram (paylasimli bellek + atomicAdd)
// 3)   BGR -> Gri donusumu
// 4)   BGR -> RGB / HSV / YCrCb / XYZ donusumleri (fonksiyon pointer'lari ile)
// Sonuclar OpenCV'nin CPU fonksiyonlari (calcHist, cvtColor) ile karsilastirilir.
// Beklenen hata degeri: 0

#include <opencv2/opencv.hpp>
#include <cuda_runtime.h>
#include <cstdio>
#include <string>

using namespace cv;

#define CUDA_CHECK(call)                                                        \
    do {                                                                        \
        cudaError_t e = (call);                                                 \
        if (e != cudaSuccess) {                                                 \
            printf("CUDA hatasi %s:%d: %s\n", __FILE__, __LINE__,               \
                   cudaGetErrorString(e));                                      \
            exit(1);                                                            \
        }                                                                       \
    } while (0)

// ---------------------------------------------------------------------------
// Cihaz (GPU) yardimcilari
// ---------------------------------------------------------------------------
__device__ __forceinline__ int descale(int x, int n) { return (x + (1 << (n - 1))) >> n; }
__device__ __forceinline__ unsigned char sat8(int v) { return (unsigned char)min(max(v, 0), 255); }

// HSV bolme tablolari (CPU'da hesaplanip sabit bellege kopyalanir)
__constant__ int c_sdiv[256];
__constant__ int c_hdiv[256];

// ---------------------------------------------------------------------------
// GOREV 1-2: Histogram cekirdegi
// Her blok kendi 256 kutulu histogramini paylasimli bellekte tutar,
// sonra global histograma ekler (atomik islemler).
// ---------------------------------------------------------------------------
__global__ void histKernel(const unsigned char* img, int numPixels, int channels,
                           int channel, unsigned int* hist)
{
    __shared__ unsigned int local[256];
    for (int i = threadIdx.x; i < 256; i += blockDim.x) local[i] = 0;
    __syncthreads();

    int stride = blockDim.x * gridDim.x;
    for (int i = blockIdx.x * blockDim.x + threadIdx.x; i < numPixels; i += stride)
        atomicAdd(&local[img[i * channels + channel]], 1u);
    __syncthreads();

    for (int i = threadIdx.x; i < 256; i += blockDim.x)
        if (local[i]) atomicAdd(&hist[i], local[i]);
}

// ---------------------------------------------------------------------------
// Piksel basina donusum fonksiyonlari (__device__)
// Girdi: b, g, r   Cikti: out[0..outCn-1]
// Katsayilar OpenCV'nin 8 bit sabit noktali katsayilaridir.
// ---------------------------------------------------------------------------
// Her donusum __device__ operator() iceren bir functor'dur; cekirdege sablon
// parametresi olarak verilir ve derleme aninda satir ici (inline) edilir.

struct PxGray {
__device__ void operator()(int b, int g, int r, unsigned char* out) const
{
    // 2^15 olcekli: 0.114, 0.587, 0.299
    out[0] = sat8(descale(b * 3735 + g * 19235 + r * 9798, 15));
}
};

struct PxRGB {
__device__ void operator()(int b, int g, int r, unsigned char* out) const
{
    out[0] = (unsigned char)r;
    out[1] = (unsigned char)g;
    out[2] = (unsigned char)b;
}
};

struct PxYCrCb {
__device__ void operator()(int b, int g, int r, unsigned char* out) const
{
    int Y  = descale(b * 1868 + g * 9617 + r * 4899, 14);
    int Cr = descale((r - Y) * 11682 + (128 << 14), 14);
    int Cb = descale((b - Y) * 9241 + (128 << 14), 14);
    out[0] = sat8(Y);
    out[1] = sat8(Cr);
    out[2] = sat8(Cb);
}
};

struct PxHSV {
__device__ void operator()(int b, int g, int r, unsigned char* out) const
{
    const int SHIFT = 12;
    int v = max(b, max(g, r));
    int vmin = min(b, min(g, r));
    int diff = v - vmin;

    int s = (diff * c_sdiv[v] + (1 << (SHIFT - 1))) >> SHIFT;

    int h;
    if (v == r)      h = g - b;
    else if (v == g) h = b - r + 2 * diff;
    else             h = r - g + 4 * diff;
    h = (h * c_hdiv[diff] + (1 << (SHIFT - 1))) >> SHIFT;
    if (h < 0) h += 180;

    out[0] = sat8(h);
    out[1] = (unsigned char)s;
    out[2] = (unsigned char)v;
}
};

struct PxXYZ {
__device__ void operator()(int b, int g, int r, unsigned char* out) const
{
    out[0] = sat8(descale(r * 1689 + g * 1465 + b * 739, 12));
    out[1] = sat8(descale(r * 871 + g * 2929 + b * 296, 12));
    out[2] = sat8(descale(r * 79 + g * 488 + b * 3892, 12));
}
};

// Genel donusum cekirdegi: hangi donusumun yapilacagi sablon parametresi olarak
// verilen functor ile belirlenir. 2B izgara: her thread bir piksel.
template <class F, int OUT_CN>
__global__ void convertKernel(const unsigned char* src, size_t srcStep,
                              unsigned char* dst, size_t dstStep, int width, int height)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;
    if (x >= width || y >= height) return;

    const unsigned char* s = src + y * srcStep + x * 3;
    unsigned char* d = dst + y * dstStep + x * OUT_CN;
    F()(s[0], s[1], s[2], d);
}

// ---------------------------------------------------------------------------
// Host tarafi: GPU bellek yonetimi ve cekirdek cagrilari
// ---------------------------------------------------------------------------
struct GpuImage {
    unsigned char* ptr = nullptr;
    size_t step = 0;
    int width = 0, height = 0, cn = 0;
};

static GpuImage upload(const Mat& m)
{
    GpuImage g;
    g.width = m.cols; g.height = m.rows; g.cn = m.channels();
    CUDA_CHECK(cudaMallocPitch(&g.ptr, &g.step, m.cols * m.elemSize(), m.rows));
    CUDA_CHECK(cudaMemcpy2D(g.ptr, g.step, m.data, m.step, m.cols * m.elemSize(), m.rows,
                            cudaMemcpyHostToDevice));
    return g;
}

// Sablon cekirdegi calistiran "launcher" fonksiyonu
template <class F, int OUT_CN>
static void launchConvert(const GpuImage& src, Mat& dst, float* ms)
{
    GpuImage d;
    d.width = src.width; d.height = src.height; d.cn = OUT_CN;
    CUDA_CHECK(cudaMallocPitch(&d.ptr, &d.step, src.width * OUT_CN, src.height));

    dim3 block(16, 16);
    dim3 grid((src.width + block.x - 1) / block.x, (src.height + block.y - 1) / block.y);

    cudaEvent_t t0, t1;
    cudaEventCreate(&t0); cudaEventCreate(&t1);
    cudaEventRecord(t0);
    convertKernel<F, OUT_CN><<<grid, block>>>(src.ptr, src.step, d.ptr, d.step, src.width, src.height);
    cudaEventRecord(t1);
    CUDA_CHECK(cudaGetLastError());
    CUDA_CHECK(cudaEventSynchronize(t1));
    cudaEventElapsedTime(ms, t0, t1);
    cudaEventDestroy(t0); cudaEventDestroy(t1);

    dst.create(src.height, src.width, CV_8UC(OUT_CN));
    CUDA_CHECK(cudaMemcpy2D(dst.data, dst.step, d.ptr, d.step, src.width * OUT_CN, src.height,
                            cudaMemcpyDeviceToHost));
    cudaFree(d.ptr);
}

// Fonksiyon pointer tablosu (host): her donusum icin GPU launcher'i + OpenCV kodu
typedef void (*GpuConvertFn)(const GpuImage& src, Mat& dst, float* ms);

struct Conversion {
    const char* name;
    GpuConvertFn fn;
    int cvCode;
};

static const Conversion conversions[] = {
    { "BGR -> GRAY ", launchConvert<PxGray, 1>,  COLOR_BGR2GRAY  },
    { "BGR -> RGB  ", launchConvert<PxRGB, 3>,   COLOR_BGR2RGB   },
    { "BGR -> HSV  ", launchConvert<PxHSV, 3>,   COLOR_BGR2HSV   },
    { "BGR -> YCrCb", launchConvert<PxYCrCb, 3>, COLOR_BGR2YCrCb },
    { "BGR -> XYZ  ", launchConvert<PxXYZ, 3>,   COLOR_BGR2XYZ   },
};

static double absDiffSum(const Mat& a, const Mat& b)
{
    Mat d;
    absdiff(a, b, d);
    Scalar s = sum(d);
    return s[0] + s[1] + s[2] + s[3];
}

// ---------------------------------------------------------------------------
int main(int argc, char** argv)
{
    std::string path = argc > 1 ? argv[1] : "resim.jpg";
    Mat img = imread(path, IMREAD_COLOR);
    if (img.empty()) {
        printf("Goruntu okunamadi: %s\n", path.c_str());
        return 1;
    }
    if (!img.isContinuous()) img = img.clone();

    cudaDeviceProp prop;
    CUDA_CHECK(cudaGetDeviceProperties(&prop, 0));
    printf("GPU: %s\n", prop.name);
    printf("Goruntu: %s  (%d x %d, %d kanal)\n\n", path.c_str(), img.cols, img.rows, img.channels());

    // HSV tablolarini hazirla (OpenCV ile ayni formul) ve sabit bellege kopyala
    int sdiv[256], hdiv[256];
    sdiv[0] = hdiv[0] = 0;
    for (int i = 1; i < 256; i++) {
        sdiv[i] = saturate_cast<int>((255 << 12) / (1.0 * i));
        hdiv[i] = saturate_cast<int>((180 << 12) / (6.0 * i));
    }
    CUDA_CHECK(cudaMemcpyToSymbol(c_sdiv, sdiv, sizeof(sdiv)));
    CUDA_CHECK(cudaMemcpyToSymbol(c_hdiv, hdiv, sizeof(hdiv)));

    // ---------------- GOREV 1-2: Histogram (GPU) ----------------
    printf("=== GOREV 1-2: CUDA histogram vs OpenCV calcHist ===\n");
    const int numPixels = img.rows * img.cols;
    unsigned char* dImg;
    unsigned int* dHist;
    CUDA_CHECK(cudaMalloc(&dImg, img.total() * img.elemSize()));
    CUDA_CHECK(cudaMemcpy(dImg, img.data, img.total() * img.elemSize(), cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMalloc(&dHist, 256 * sizeof(unsigned int)));

    std::vector<Mat> ch;
    split(img, ch);
    const char* chName[3] = { "B", "G", "R" };
    int histSize = 256;
    float range[] = { 0, 256 };
    const float* ranges[] = { range };

    for (int c = 0; c < 3; c++) {
        CUDA_CHECK(cudaMemset(dHist, 0, 256 * sizeof(unsigned int)));
        int threads = 256;
        int blocks = std::min((numPixels + threads - 1) / threads, prop.multiProcessorCount * 8);
        histKernel<<<blocks, threads>>>(dImg, numPixels, 3, c, dHist);
        CUDA_CHECK(cudaGetLastError());

        unsigned int h[256];
        CUDA_CHECK(cudaMemcpy(h, dHist, sizeof(h), cudaMemcpyDeviceToHost));
        Mat gpuHist(256, 1, CV_32F);
        for (int i = 0; i < 256; i++) gpuHist.at<float>(i) = (float)h[i];

        Mat cvHist;
        calcHist(&ch[c], 1, 0, Mat(), cvHist, 1, &histSize, ranges);
        printf("  %s kanali: mutlak fark toplami = %.0f\n", chName[c], norm(cvHist, gpuHist, NORM_L1));
    }
    cudaFree(dHist);
    cudaFree(dImg);
    printf("\n");

    // ---------------- GOREV 3-4: Renk donusumleri (GPU) ----------------
    printf("=== GOREV 3-4: CUDA renk donusumleri vs OpenCV cvtColor ===\n");
    printf("  %-14s %-22s %-15s %s\n", "Donusum", "Mutlak fark toplami", "Farkli piksel", "Cekirdek suresi");
    GpuImage gImg = upload(img);
    for (const Conversion& conv : conversions) {
        Mat gpuOut, ref;
        float ms = 0;
        conv.fn(gImg, gpuOut, &ms);            // fonksiyon pointer'i ile GPU donusumu
        cvtColor(img, ref, conv.cvCode);

        Mat neq = gpuOut != ref;
        neq = neq.reshape(1);
        printf("  %-14s %-22.0f %-15d %.3f ms\n", conv.name, absDiffSum(gpuOut, ref),
               countNonZero(neq), ms);
    }
    cudaFree(gImg.ptr);

    printf("\nBeklenen hata degeri: 0\n");
    return 0;
}
