// Hafta 5 - Odev 4: Renk uzayi donusumleri (fonksiyon pointer'lari ile)
// Bir renk uzayindaki goruntuyu diger renk uzaylarina donusturen fonksiyonlar yazilir.
// Fonksiyonlarin hepsi ayni imzaya sahiptir ve bir fonksiyon pointer tablosu
// uzerinden cagrilir. Her ciktinin OpenCV'nin cvtColor ciktisiyla farki
// sayisal olarak hesaplanir. Beklenen hata degeri: 0

#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <string>

using namespace cv;
using namespace std;

// Tum donusum fonksiyonlarinin ortak tipi (fonksiyon pointer'i)
typedef void (*DonusumFonk)(const Mat& giris, Mat& cikis);

// OpenCV'nin sabit noktali yuvarlamasi: (x + 2^(n-1)) >> n
inline int olcekle(int x, int n) { return (x + (1 << (n - 1))) >> n; }

// ---------------------------------------------------------------------------
// BGR -> GRAY:  Gri = 0.299 R + 0.587 G + 0.114 B  (2^15 olcekli katsayilar)
// ---------------------------------------------------------------------------
void bgrGri(const Mat& giris, Mat& cikis)
{
    cikis.create(giris.size(), CV_8UC1);
    for (int y = 0; y < giris.rows; y++)
    {
        const Vec3b* g = giris.ptr<Vec3b>(y);
        uchar* c = cikis.ptr<uchar>(y);
        for (int x = 0; x < giris.cols; x++)
            c[x] = saturate_cast<uchar>(olcekle(g[x][0] * 3735 + g[x][1] * 19235 + g[x][2] * 9798, 15));
    }
}

// ---------------------------------------------------------------------------
// BGR -> RGB:  kanal sirasi ters cevrilir
// ---------------------------------------------------------------------------
void bgrRgb(const Mat& giris, Mat& cikis)
{
    cikis.create(giris.size(), CV_8UC3);
    for (int y = 0; y < giris.rows; y++)
    {
        const Vec3b* g = giris.ptr<Vec3b>(y);
        Vec3b* c = cikis.ptr<Vec3b>(y);
        for (int x = 0; x < giris.cols; x++)
            c[x] = Vec3b(g[x][2], g[x][1], g[x][0]);
    }
}

// ---------------------------------------------------------------------------
// BGR -> HSV
// V = max(R,G,B)
// S = (V - min) / V * 255
// H = 0-180 araliginda (8 bitte 360 derece sigmadigi icin derece / 2)
// OpenCV bolmeleri onceden hesaplanmis tablolarla (2^12 olcekli) yapar.
// ---------------------------------------------------------------------------
void bgrHsv(const Mat& giris, Mat& cikis)
{
    const int KAY = 12;
    static int sTablo[256], hTablo[256];
    static bool hazir = false;
    if (!hazir)
    {
        sTablo[0] = hTablo[0] = 0;
        for (int i = 1; i < 256; i++)
        {
            sTablo[i] = saturate_cast<int>((255 << KAY) / (1.0 * i));       // 255 / i
            hTablo[i] = saturate_cast<int>((180 << KAY) / (6.0 * i));       // 30 / i
        }
        hazir = true;
    }

    cikis.create(giris.size(), CV_8UC3);
    for (int y = 0; y < giris.rows; y++)
    {
        const Vec3b* g = giris.ptr<Vec3b>(y);
        Vec3b* c = cikis.ptr<Vec3b>(y);
        for (int x = 0; x < giris.cols; x++)
        {
            int b = g[x][0], gr = g[x][1], r = g[x][2];
            int v = max(b, max(gr, r));
            int enKucuk = min(b, min(gr, r));
            int fark = v - enKucuk;

            int s = (fark * sTablo[v] + (1 << (KAY - 1))) >> KAY;

            int h;
            if (v == r)       h = gr - b;               // kirmizi baskin: 0 derece civari
            else if (v == gr) h = b - r + 2 * fark;     // yesil baskin: 120 derece civari
            else              h = r - gr + 4 * fark;    // mavi baskin: 240 derece civari
            h = (h * hTablo[fark] + (1 << (KAY - 1))) >> KAY;
            if (h < 0) h += 180;

            c[x] = Vec3b(saturate_cast<uchar>(h), (uchar)s, (uchar)v);
        }
    }
}

// ---------------------------------------------------------------------------
// BGR -> YCrCb
// Y  = 0.299 R + 0.587 G + 0.114 B
// Cr = (R - Y) * 0.713 + 128
// Cb = (B - Y) * 0.564 + 128      (2^14 olcekli katsayilar)
// ---------------------------------------------------------------------------
void bgrYCrCb(const Mat& giris, Mat& cikis)
{
    const int KAY = 14;
    const int ORTA = 128 << KAY;
    cikis.create(giris.size(), CV_8UC3);
    for (int y = 0; y < giris.rows; y++)
    {
        const Vec3b* g = giris.ptr<Vec3b>(y);
        Vec3b* c = cikis.ptr<Vec3b>(y);
        for (int x = 0; x < giris.cols; x++)
        {
            int B = g[x][0], G = g[x][1], R = g[x][2];
            int Y = olcekle(B * 1868 + G * 9617 + R * 4899, KAY);
            int Cr = olcekle((R - Y) * 11682 + ORTA, KAY);   // 0.713 * 16384
            int Cb = olcekle((B - Y) * 9241 + ORTA, KAY);    // 0.564 * 16384
            c[x] = Vec3b(saturate_cast<uchar>(Y), saturate_cast<uchar>(Cr), saturate_cast<uchar>(Cb));
        }
    }
}

// ---------------------------------------------------------------------------
// BGR -> XYZ (sRGB, D65 beyaz noktasi)
// | X |   | 0.412453 0.357580 0.180423 |   | R |
// | Y | = | 0.212671 0.715160 0.072169 | * | G |
// | Z |   | 0.019334 0.119193 0.950227 |   | B |      (2^12 olcekli)
// ---------------------------------------------------------------------------
void bgrXyz(const Mat& giris, Mat& cikis)
{
    const int KAY = 12;
    static const int M[9] = { 1689, 1465,  739,
                               871, 2929,  296,
                                79,  488, 3892 };
    cikis.create(giris.size(), CV_8UC3);
    for (int y = 0; y < giris.rows; y++)
    {
        const Vec3b* g = giris.ptr<Vec3b>(y);
        Vec3b* c = cikis.ptr<Vec3b>(y);
        for (int x = 0; x < giris.cols; x++)
        {
            int B = g[x][0], G = g[x][1], R = g[x][2];
            int X = olcekle(R * M[0] + G * M[1] + B * M[2], KAY);
            int Y = olcekle(R * M[3] + G * M[4] + B * M[5], KAY);
            int Z = olcekle(R * M[6] + G * M[7] + B * M[8], KAY);
            c[x] = Vec3b(saturate_cast<uchar>(X), saturate_cast<uchar>(Y), saturate_cast<uchar>(Z));
        }
    }
}

// Fonksiyon pointer tablosu: ad, bizim fonksiyonumuz, karsilik gelen OpenCV kodu, cikti dosyasi
struct Donusum
{
    const char* ad;
    DonusumFonk fonk;
    int opencvKodu;
    const char* dosya;
};

const Donusum donusumler[] = {
    { "BGR -> GRAY",  bgrGri,   COLOR_BGR2GRAY,  "cikti_gray.png"  },
    { "BGR -> RGB",   bgrRgb,   COLOR_BGR2RGB,   "cikti_rgb.png"   },
    { "BGR -> HSV",   bgrHsv,   COLOR_BGR2HSV,   "cikti_hsv.png"   },
    { "BGR -> YCrCb", bgrYCrCb, COLOR_BGR2YCrCb, "cikti_ycrcb.png" },
    { "BGR -> XYZ",   bgrXyz,   COLOR_BGR2XYZ,   "cikti_xyz.png"   },
};

int main()
{
    // OpenCV'nin INFO mesajlarini kapat
    utils::logging::setLogLevel(utils::logging::LOG_LEVEL_ERROR);

    Mat kaynak = imread("resim.jpg", IMREAD_COLOR);
    if (kaynak.empty())
    {
        cout << "resim.jpg bulunamadi!" << endl;
        return -1;
    }
    cout << "Goruntu: " << kaynak.cols << " x " << kaynak.rows
        << " (" << kaynak.total() << " piksel)" << endl << endl;

    cout << left << setw(14) << "Donusum" << setw(24) << "Mutlak fark toplami"
        << "Farkli deger sayisi" << endl;

    for (const Donusum& d : donusumler)
    {
        Mat benim, opencv;
        d.fonk(kaynak, benim);                    // fonksiyon pointer'i uzerinden cagri
        cvtColor(kaynak, opencv, d.opencvKodu);   // OpenCV'nin hazir fonksiyonu

        Mat fark;
        absdiff(benim, opencv, fark);
        Scalar s = sum(fark);
        double hata = s[0] + s[1] + s[2];
        int farkliSayisi = countNonZero(fark.reshape(1));

        cout << left << setw(14) << d.ad << setw(24) << hata << farkliSayisi
            << (hata == 0 ? "  (dogru)" : "  (HATA)") << endl;

        imshow(d.ad, benim);
        imwrite(d.dosya, benim);
    }
    cout << "\nBeklenen hata degeri: 0" << endl;

    imshow("Orijinal", kaynak);
    waitKey(0);
    return 0;
}
