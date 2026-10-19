// CUDA destekli OpenCV testi:
// Bir dizindeki goruntuleri okuyup GPU uzerinde 1024x768'e boyutlandirir,
// sonucu CPU'daki resize ile karsilastirir.
#include <opencv2/opencv.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/cudawarping.hpp>
#include <iostream>
#include <filesystem>
#include <chrono>

using namespace cv;
using namespace std;
namespace fs = std::filesystem;

int main(int argc, char** argv)
{
    int gpuSayisi = cuda::getCudaEnabledDeviceCount();
    cout << "CUDA destekli GPU sayisi: " << gpuSayisi << endl;
    if (gpuSayisi == 0) { cout << "OpenCV CUDA destegi olmadan derlenmis!" << endl; return -1; }
    cuda::printShortCudaDeviceInfo(cuda::getDevice());

    fs::path giris = argc > 1 ? argv[1] : "resimler";
    fs::path cikis = argc > 2 ? argv[2] : "cikti_1024x768";
    fs::create_directories(cikis);

    for (const auto& oge : fs::directory_iterator(giris))
    {
        Mat resim = imread(oge.path().string());
        if (resim.empty()) continue;

        // GPU: yukle -> boyutlandir -> indir
        auto t1 = chrono::high_resolution_clock::now();
        cuda::GpuMat gGiris, gCikis;
        gGiris.upload(resim);
        cuda::resize(gGiris, gCikis, Size(1024, 768), 0, 0, INTER_LINEAR);
        Mat gpuSonuc;
        gCikis.download(gpuSonuc);
        auto t2 = chrono::high_resolution_clock::now();

        // CPU: ayni islem
        Mat cpuSonuc;
        resize(resim, cpuSonuc, Size(1024, 768), 0, 0, INTER_LINEAR);
        auto t3 = chrono::high_resolution_clock::now();

        imwrite((cikis / oge.path().filename()).string(), gpuSonuc);

        // GPU ve CPU sonuclari arasindaki fark (en buyuk ve ortalama)
        Mat fark;
        absdiff(gpuSonuc, cpuSonuc, fark);
        Scalar ort = mean(fark);
        double ortalama = (ort[0] + ort[1] + ort[2]) / resim.channels();
        cout << oge.path().filename().string() << ": " << resim.cols << "x" << resim.rows
             << " -> " << gpuSonuc.cols << "x" << gpuSonuc.rows
             << " | GPU " << chrono::duration<double, milli>(t2 - t1).count() << " ms"
             << ", CPU " << chrono::duration<double, milli>(t3 - t2).count() << " ms"
             << " | fark: en buyuk " << norm(gpuSonuc, cpuSonuc, NORM_INF)
             << ", ortalama " << ortalama << endl;
    }
    return 0;
}