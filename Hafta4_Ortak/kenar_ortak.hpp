// Hafta 4 - Kenar bulma gorevlerinde ortak kullanilan fonksiyonlar
// (cekirdekler, gradyan buyuklugu, sifir gecisi, sentetik test goruntusu,
//  dogruluk olcumu ve sure olcumu)

#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <functional>
#include <string>

using namespace cv;
using namespace std;

// ---------------------------------------------------------------------------
// Cekirdekler
// ---------------------------------------------------------------------------
// 9 elemanli diziden 3x3 ondalikli cekirdek olusturur
inline Mat cekirdek3x3(const float (&d)[9]) { return Mat(3, 3, CV_32F, (void*)d).clone(); }

inline Mat prewittX() { const float d[9] = { -1, 0, 1, -1, 0, 1, -1, 0, 1 };  return cekirdek3x3(d); }
inline Mat prewittY() { const float d[9] = { -1, -1, -1, 0, 0, 0, 1, 1, 1 };  return cekirdek3x3(d); }
inline Mat sobelX()   { const float d[9] = { -1, 0, 1, -2, 0, 2, -1, 0, 1 };  return cekirdek3x3(d); }
inline Mat sobelY()   { const float d[9] = { -1, -2, -1, 0, 0, 0, 1, 2, 1 };  return cekirdek3x3(d); }
inline Mat laplace3() { const float d[9] = { 0, 1, 0, 1, -4, 1, 0, 1, 0 };    return cekirdek3x3(d); }

// 7x7 Laplacian of Gaussian cekirdegi
// LoG(x,y) = -1/(pi*s^4) * (1 - r^2/(2s^2)) * exp(-r^2/(2s^2))
// Toplami 0 olacak sekilde ortalamasi cikarilir (duz bolgede cevap 0 olsun diye).
inline Mat logCekirdegi(int boyut = 7, double sigma = 1.0)
{
    Mat k(boyut, boyut, CV_32F);
    int m = boyut / 2;
    double s2 = sigma * sigma;
    for (int y = -m; y <= m; y++)
        for (int x = -m; x <= m; x++)
        {
            double r2 = x * x + y * y;
            k.at<float>(y + m, x + m) =
                (float)(-1.0 / (CV_PI * s2 * s2) * (1 - r2 / (2 * s2)) * exp(-r2 / (2 * s2)));
        }
    k -= mean(k)[0];
    return k;
}

// ---------------------------------------------------------------------------
// Temel islemler
// ---------------------------------------------------------------------------
inline Mat griOku(const string& yol)
{
    Mat g = imread(yol, IMREAD_GRAYSCALE);
    if (g.empty()) { cout << yol << " bulunamadi!" << endl; exit(-1); }
    return g;
}

// "Hafif" yumusatma: 3x3 Gauss, sigma = 0.8
inline Mat hafifYumusat(const Mat& g)
{
    Mat s;
    GaussianBlur(g, s, Size(3, 3), 0.8);
    return s;
}

// Cekirdegi goruntu uzerinde gezdirir (sonuc ondalikli, isaretli)
inline Mat filtrele(const Mat& g, const Mat& cekirdek)
{
    Mat s;
    filter2D(g, s, CV_32F, cekirdek, Point(-1, -1), 0, BORDER_REFLECT);
    return s;
}

// Gradyan buyuklugu: sqrt(gx^2 + gy^2) / olcek
// olcek: Prewitt icin 3, Sobel icin 4 -> sonuclar 0-255 araliginda karsilastirilabilir
inline Mat gradyanBuyuklugu(const Mat& g, const Mat& kx, const Mat& ky, float olcek)
{
    Mat gx = filtrele(g, kx), gy = filtrele(g, ky), b;
    magnitude(gx, gy, b);
    return b / olcek;
}

// Isaretli bir cevabi gosterim icin 0-255'e cevirir (mutlak deger)
inline Mat gosterim(const Mat& cevap, double olcek = 1.0)
{
    Mat s;
    convertScaleAbs(cevap, s, olcek);
    return s;
}

// Laplacian / LoG cevabindaki sifir gecislerinin gucu.
// Bir pikselin karsilikli iki komsusu (yatay, dikey, iki capraz) zit isaretliyse
// piksel sifir gecisi uzerindedir; iki komsu arasindaki fark kenar gucudur.
inline Mat sifirGecisGucu(const Mat& L)
{
    Mat guc = Mat::zeros(L.size(), CV_32F);
    for (int y = 1; y < L.rows - 1; y++)
    {
        const float* ust = L.ptr<float>(y - 1);
        const float* orta = L.ptr<float>(y);
        const float* alt = L.ptr<float>(y + 1);
        float* g = guc.ptr<float>(y);
        for (int x = 1; x < L.cols - 1; x++)
        {
            // karsilikli komsu ciftleri: yatay, dikey, iki capraz
            float b[4] = { orta[x + 1], alt[x], alt[x + 1], ust[x + 1] };
            float c[4] = { orta[x - 1], ust[x], ust[x - 1], alt[x - 1] };
            float enBuyuk = 0;
            for (int k = 0; k < 4; k++)
                if (b[k] * c[k] < 0) enBuyuk = max(enBuyuk, fabs(b[k] - c[k]));
            g[x] = enBuyuk;
        }
    }
    return guc;
}

inline Mat esikle(const Mat& guc, double esik)
{
    Mat k = guc > esik;   // 0 / 255
    return k;
}

// ---------------------------------------------------------------------------
// Dogruluk olcumu icin sentetik test goruntusu
// Kenarlarin yeri bilindigi icin her yontemin dogrulugu olculebilir.
// ---------------------------------------------------------------------------
inline void sentetikGoruntu(Mat& temiz, Mat& gurultulu, Mat& gercekKenar, double gurultuSigma = 12)
{
    temiz = Mat(400, 400, CV_8U, Scalar(100));
    rectangle(temiz, Rect(40, 40, 140, 100), Scalar(180), FILLED);
    circle(temiz, Point(290, 110), 65, Scalar(40), FILLED);
    vector<Point> ucgen = { Point(70, 360), Point(170, 200), Point(250, 360) };
    fillConvexPoly(temiz, ucgen, Scalar(200));
    RotatedRect rr(Point2f(320, 290), Size2f(110, 70), 30);
    Point2f p[4]; rr.points(p);
    vector<Point> dortgen(p, p + 4);
    fillConvexPoly(temiz, dortgen, Scalar(150));

    // Gercek kenar: degerin degistigi piksel sinirlari
    morphologyEx(temiz, gercekKenar, MORPH_GRADIENT, getStructuringElement(MORPH_RECT, Size(3, 3)));
    gercekKenar = gercekKenar > 0;

    RNG rng(12345);
    Mat gurultu(temiz.size(), CV_32F);
    rng.fill(gurultu, RNG::NORMAL, 0, gurultuSigma);
    Mat t; temiz.convertTo(t, CV_32F);
    Mat toplam = t + gurultu;
    toplam.convertTo(gurultulu, CV_8U);
}

struct Dogruluk { double kesinlik, duyarlilik, f1; };

// 2 piksel toleransla kesinlik / duyarlilik / F1
inline Dogruluk dogrulukOlc(const Mat& bulunan, const Mat& gercek, int tolerans = 2)
{
    Mat el = getStructuringElement(MORPH_RECT, Size(2 * tolerans + 1, 2 * tolerans + 1));
    Mat gercekGenis, bulunanGenis;
    dilate(gercek, gercekGenis, el);
    dilate(bulunan, bulunanGenis, el);
    double nB = countNonZero(bulunan), nG = countNonZero(gercek);
    double dogruB = countNonZero(bulunan & gercekGenis);
    double yakalananG = countNonZero(gercek & bulunanGenis);
    Dogruluk d;
    d.kesinlik = nB ? dogruB / nB : 0;
    d.duyarlilik = nG ? yakalananG / nG : 0;
    d.f1 = (d.kesinlik + d.duyarlilik) ? 2 * d.kesinlik * d.duyarlilik / (d.kesinlik + d.duyarlilik) : 0;
    return d;
}

// 0..ust araliginda 99 esik dener, F1'i en yuksek yapan esigi bulur.
// Birden fazla esik ayni en iyi F1'i veriyorsa bu araligin ortasi secilir.
inline Dogruluk enIyiEsikAra(const function<Mat(double)>& kenarUret, double ust,
                             const Mat& gercek, double* esikCikti = nullptr)
{
    double enIyiF1 = -1, bas = 0, son = 0;
    for (int i = 1; i < 100; i++)
    {
        double t = ust * i / 100.0;
        double f1 = dogrulukOlc(kenarUret(t), gercek).f1;
        if (f1 > enIyiF1 + 1e-9) { enIyiF1 = f1; bas = son = t; }
        else if (fabs(f1 - enIyiF1) <= 1e-9 && son == ust * (i - 1) / 100.0) son = t;
    }
    double esik = (bas + son) / 2;
    if (esikCikti) *esikCikti = esik;
    return dogrulukOlc(kenarUret(esik), gercek);
}

// Kenar gucu haritasi icin F1'i en yuksek yapan esigi arar
inline Dogruluk enIyiEsikle(const Mat& guc, const Mat& gercek, double* esikCikti = nullptr)
{
    double enBuyuk;
    minMaxLoc(guc, nullptr, &enBuyuk);
    return enIyiEsikAra([&](double t) { return esikle(guc, t); }, enBuyuk, gercek, esikCikti);
}

// "Gauss + Sobel" -> "Gauss_Sobel" (dosya adi icin)
inline string dosyaAdi(string ad)
{
    size_t p;
    while ((p = ad.find(" + ")) != string::npos) ad.replace(p, 3, "_");
    replace(ad.begin(), ad.end(), ' ', '_');
    return ad;
}

// Bir islemin ortalama calisma suresi (ms)
inline double sureOlc(const function<void()>& is, int tekrar = 30)
{
    is();   // isinma
    auto t1 = chrono::high_resolution_clock::now();
    for (int i = 0; i < tekrar; i++) is();
    auto t2 = chrono::high_resolution_clock::now();
    return chrono::duration<double, milli>(t2 - t1).count() / tekrar;
}
