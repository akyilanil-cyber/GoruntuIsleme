// Hafta 3 - Gorev 2: Elle Histogram Esitleme (Histogram Equalization)
// Dusuk kontrastli, tek kanalli bir goruntu acilir.
// 256 degerli histogram, kumulatif dagilim (CDF) ve esitleme
// HAZIR FONKSIYON KULLANILMADAN (calcHist / equalizeHist yok) hesaplanir.

#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <io.h>

using namespace cv;
using namespace std;

// Klasordeki ilk resim dosyasini (adi ne olursa olsun) bulup gri olarak acar
Mat griResimAc()
{
    vector<string> uzantilar = { ".jpg", ".jpeg", ".png", ".bmp", ".webp" };
    _finddata_t bilgi;
    intptr_t h = _findfirst("*.*", &bilgi);
    if (h == -1) return Mat();

    Mat sonuc;
    do
    {
        string ad = bilgi.name;
        string kucuk = ad;
        transform(kucuk.begin(), kucuk.end(), kucuk.begin(), ::tolower);

        bool resimMi = false;
        for (const string& u : uzantilar)
            if (kucuk.size() >= u.size() && kucuk.compare(kucuk.size() - u.size(), u.size(), u) == 0)
                resimMi = true;
        if (!resimMi) continue;

        // Kendi ciktilarimizi tekrar okumayalim
        if (kucuk.find("esitlenmis") != string::npos || kucuk.find("histogram") != string::npos) continue;

        ifstream dosya(ad, ios::binary);
        vector<uchar> veri((istreambuf_iterator<char>(dosya)), istreambuf_iterator<char>());
        Mat m = imdecode(veri, IMREAD_GRAYSCALE);
        if (!m.empty())
        {
            cout << "Kullanilan resim: " << ad << endl;
            sonuc = m;
            break;
        }
    } while (_findnext(h, &bilgi) == 0);
    _findclose(h);

    if (sonuc.empty())
        cout << "Klasorde okunabilir bir resim bulunamadi! Resmi Hafta3_Odev2.cpp'nin yanina koy." << endl;
    return sonuc;
}

// 1) Histogram: her parlaklik degerinden (0-255) kac piksel oldugunu sayar
void histogramHesapla(const Mat& gri, long long hist[256])
{
    for (int i = 0; i < 256; i++) hist[i] = 0;
    for (int y = 0; y < gri.rows; y++)
    {
        const uchar* satir = gri.ptr<uchar>(y);
        for (int x = 0; x < gri.cols; x++)
            hist[satir[x]]++;
    }
}

// Histogrami cubuk grafik olarak cizer (sadece gorsellestirme)
Mat histogramCiz(const long long hist[256], const string& baslik)
{
    int genislik = 512, yukseklik = 300;
    Mat resim(yukseklik + 30, genislik, CV_8UC3, Scalar(255, 255, 255));

    long long enBuyuk = 1;
    for (int i = 0; i < 256; i++) if (hist[i] > enBuyuk) enBuyuk = hist[i];

    for (int i = 0; i < 256; i++)
    {
        int boy = (int)((double)hist[i] / enBuyuk * yukseklik);
        rectangle(resim, Point(i * 2, yukseklik + 30 - boy),
            Point(i * 2 + 1, yukseklik + 30), Scalar(80, 80, 80), FILLED);
    }
    putText(resim, baslik, Point(10, 20), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 0, 255), 2);
    return resim;
}

// Histogramdan en kucuk ve en buyuk parlaklik degerini bulur
void aralikBul(const long long hist[256], int& enKucuk, int& enBuyuk)
{
    enKucuk = 0; enBuyuk = 255;
    while (enKucuk < 255 && hist[enKucuk] == 0) enKucuk++;
    while (enBuyuk > 0 && hist[enBuyuk] == 0) enBuyuk--;
}

int main()
{
    utils::logging::setLogLevel(utils::logging::LOG_LEVEL_ERROR);

    // Goruntuyu TEK KANALLI (gri) olarak ac
    Mat gri = griResimAc();
    if (gri.empty()) return -1;

    long long N = (long long)gri.rows * gri.cols;
    cout << "Goruntu: " << gri.cols << " x " << gri.rows << " (" << N << " piksel)" << endl;

    // 1) Histogram
    long long hist[256];
    histogramHesapla(gri, hist);

    // 2) Kumulatif dagilim (CDF): cdf[v] = 0..v arasindaki piksellerin toplami
    long long cdf[256];
    long long toplam = 0;
    for (int v = 0; v < 256; v++)
    {
        toplam += hist[v];
        cdf[v] = toplam;
    }

    // cdfMin: sifir olmayan ilk CDF degeri
    long long cdfMin = 0;
    for (int v = 0; v < 256; v++)
        if (cdf[v] > 0) { cdfMin = cdf[v]; break; }

    // 3) Esitleme tablosu: yeni = round( (cdf[v] - cdfMin) / (N - cdfMin) * 255 )
    uchar tablo[256];
    for (int v = 0; v < 256; v++)
    {
        if (N == cdfMin) { tablo[v] = (uchar)v; continue; }
        double oran = (double)(cdf[v] - cdfMin) / (double)(N - cdfMin);
        tablo[v] = saturate_cast<uchar>(oran * 255.0);
    }

    // 4) Tabloyu her piksele uygula
    Mat esit(gri.size(), CV_8UC1);
    for (int y = 0; y < gri.rows; y++)
    {
        const uchar* g = gri.ptr<uchar>(y);
        uchar* e = esit.ptr<uchar>(y);
        for (int x = 0; x < gri.cols; x++)
            e[x] = tablo[g[x]];
    }

    // Sonuc histogrami
    long long histYeni[256];
    histogramHesapla(esit, histYeni);

    int a1, b1, a2, b2;
    aralikBul(hist, a1, b1);
    aralikBul(histYeni, a2, b2);
    cout << "Orijinal parlaklik araligi  : " << a1 << " - " << b1 << endl;
    cout << "Esitlenmis parlaklik araligi: " << a2 << " - " << b2 << endl;

    cout << "\nDeger | Histogram | CDF | CDF (oran) | Yeni deger" << endl;
    for (int v = a1; v <= b1; v += max(1, (b1 - a1) / 10))
        cout << v << "\t" << hist[v] << "\t" << cdf[v] << "\t"
        << (double)cdf[v] / N << "\t" << (int)tablo[v] << endl;

    imwrite("esitlenmis.png", esit);
    imwrite("histogram_once.png", histogramCiz(hist, "Orijinal histogram"));
    imwrite("histogram_sonra.png", histogramCiz(histYeni, "Esitlenmis histogram"));

    imshow("Orijinal", gri);
    imshow("Esitlenmis", esit);
    imshow("Histogram - Orijinal", histogramCiz(hist, "Orijinal histogram"));
    imshow("Histogram - Esitlenmis", histogramCiz(histYeni, "Esitlenmis histogram"));

    cout << "\nesitlenmis.png kaydedildi." << endl;
    waitKey(0);
    return 0;
}