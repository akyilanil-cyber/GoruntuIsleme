// Hafta 2 - Odev 3: Parcali Histogram
// Resim 4 parcaya bolunur, her parcanin histogrami ayri thread/cekirdekte
// hesaplanir, sonra toplanarak ana resmin histogrami bulunur.
// Dogrulama: parcalardan toplanan histogram ile tum resmin tek seferde
// hesaplanan histogrami birebir ayni olmali.

#define NOMINMAX
#include <windows.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <iostream>
#include <thread>
#include <vector>
#include <string>

using namespace cv;
using namespace std;

// Bir gri seviye bolgede her parlaklik degerinden (0-255) kac piksel oldugunu sayar
void histogramHesapla(const Mat& gri, int hist[256])
{
    for (int i = 0; i < 256; i++) hist[i] = 0;

    for (int y = 0; y < gri.rows; y++)
    {
        const uchar* satir = gri.ptr<uchar>(y);
        for (int x = 0; x < gri.cols; x++)
            hist[satir[x]]++;   // piksel degeri = dizideki indeks
    }
}

// Histogrami cubuk grafik olarak cizer
Mat histogramCiz(const int hist[256], const string& baslik)
{
    int genislik = 512, yukseklik = 300;
    Mat resim(yukseklik + 30, genislik, CV_8UC3, Scalar(255, 255, 255));

    int enBuyuk = 0;
    for (int i = 0; i < 256; i++) enBuyuk = max(enBuyuk, hist[i]);
    if (enBuyuk == 0) enBuyuk = 1;

    for (int i = 0; i < 256; i++)
    {
        int boy = (int)((double)hist[i] / enBuyuk * yukseklik);
        rectangle(resim,
            Point(i * 2, yukseklik + 30 - boy),
            Point(i * 2 + 1, yukseklik + 30),
            Scalar(80, 80, 80),
            FILLED);
    }
    putText(resim, baslik, Point(10, 20), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 0, 255), 2);
    return resim;
}

int main()
{
    // OpenCV'nin INFO mesajlarini kapat, sadece hatalar gorunsun
    utils::logging::setLogLevel(utils::logging::LOG_LEVEL_ERROR);

    // 1) Resmi al ve gri seviyeye cevir (parlaklik = gri deger)
    Mat renkli = imread("resim.jpg");
    if (renkli.empty())
    {
        cout << "resim.jpg bulunamadi!" << endl;
        return -1;
    }
    Mat gri;
    cvtColor(renkli, gri, COLOR_BGR2GRAY);
    cout << "Resim boyutu: " << gri.cols << " x " << gri.rows
        << " (" << gri.total() << " piksel)" << endl;

    // 2) 4 parca: sol ust, sag ust, sol alt, sag alt
    int w2 = gri.cols / 2, h2 = gri.rows / 2;
    Rect parcalar[4] = {
        Rect(0,  0,  w2,            h2),
        Rect(w2, 0,  gri.cols - w2, h2),
        Rect(0,  h2, w2,            gri.rows - h2),
        Rect(w2, h2, gri.cols - w2, gri.rows - h2)
    };
    string parcaAdlari[4] = { "Sol ust", "Sag ust", "Sol alt", "Sag alt" };

    // 3) Her parcanin histogrami AYRI dizide, AYRI thread'de hesaplanir.
    //    Her thread sadece kendi dizisine yazdigi icin cakisma olmaz.
    int parcaHist[4][256];
    vector<thread> threadler;
    for (int i = 0; i < 4; i++)
    {
        Mat parca = gri(parcalar[i]);
        threadler.emplace_back([parca, &parcaHist, i]() {
            histogramHesapla(parca, parcaHist[i]);
            });
        SetThreadAffinityMask(threadler.back().native_handle(), (DWORD_PTR)1 << i);
    }
    for (auto& t : threadler) t.join();

    // 4) Parca histogramlarini topla -> ana resmin histogrami
    int toplamHist[256] = { 0 };
    for (int i = 0; i < 4; i++)
        for (int v = 0; v < 256; v++)
            toplamHist[v] += parcaHist[i][v];

    // 5) Dogrulama: tum resmin histogramini tek seferde hesapla, karsilastir
    int dogrudanHist[256];
    histogramHesapla(gri, dogrudanHist);

    bool ayni = true;
    long long toplamPiksel = 0;
    for (int v = 0; v < 256; v++)
    {
        if (toplamHist[v] != dogrudanHist[v]) ayni = false;
        toplamPiksel += toplamHist[v];
    }

    cout << "Parcalardan toplanan histogram = tek seferde hesaplanan? "
        << (ayni ? "EVET" : "HAYIR") << endl;
    cout << "Histogramdaki toplam piksel: " << toplamPiksel
        << " (resimdeki piksel: " << gri.total() << ")" << endl;

    // Ornek: birkac parlaklik degeri icin sayilar
    cout << "\nDeger | Sol ust | Sag ust | Sol alt | Sag alt | TOPLAM" << endl;
    for (int v : { 0, 64, 128, 192, 255 })
    {
        cout << v << "\t" << parcaHist[0][v] << "\t" << parcaHist[1][v] << "\t"
            << parcaHist[2][v] << "\t" << parcaHist[3][v] << "\t"
            << toplamHist[v] << endl;
    }

    // 6) Goster ve kaydet
    imshow("Gri resim", gri);
    for (int i = 0; i < 4; i++)
    {
        Mat ciz = histogramCiz(parcaHist[i], parcaAdlari[i]);
        imshow("Histogram - " + parcaAdlari[i], ciz);
        imwrite("hist_parca" + to_string(i + 1) + ".png", ciz);
    }
    Mat toplamCiz = histogramCiz(toplamHist, "Ana resim (toplam)");
    imshow("Histogram - Ana resim", toplamCiz);
    imwrite("hist_toplam.png", toplamCiz);

    waitKey(0);
    return 0;
}