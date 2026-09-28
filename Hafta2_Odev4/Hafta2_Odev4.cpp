// Hafta 2 - Odev 4: Dogrusal parlaklik/kontrast donusumu
// Her piksel icin:  yeni = 0.75 * eski + 20
// Goruntu 4 parcaya bolunur, her parca ayri thread/cekirdekte islenir.
// Sonuc OpenCV'nin convertTo fonksiyonuyla dogrulanir.

#define NOMINMAX
#include <windows.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

using namespace cv;
using namespace std;

const double ALFA = 0.75; // carpan (kontrast): <1 oldugu icin kontrast azalir
const double BETA = 20;   // eklenen deger (parlaklik)

// Bir bolgedeki her piksel: yeni = ALFA * eski + BETA
void donustur(const Mat& giris, Mat& cikis)
{
    for (int y = 0; y < giris.rows; y++)
    {
        const Vec3b* g = giris.ptr<Vec3b>(y);
        Vec3b* c = cikis.ptr<Vec3b>(y);
        for (int x = 0; x < giris.cols; x++)
            for (int k = 0; k < 3; k++)
                c[x][k] = saturate_cast<uchar>(ALFA * g[x][k] + BETA);
    }
}

// Goruntuyu 4 ceyrege boler, her ceyregi ayri thread + ayri cekirdekte isler
void dortCekirdekIsle(const Mat& giris, Mat& cikis)
{
    int w2 = giris.cols / 2, h2 = giris.rows / 2;
    Rect parcalar[4] = {
        Rect(0,  0,  w2,              h2),
        Rect(w2, 0,  giris.cols - w2, h2),
        Rect(0,  h2, w2,              giris.rows - h2),
        Rect(w2, h2, giris.cols - w2, giris.rows - h2)
    };

    vector<thread> threadler;
    for (int i = 0; i < 4; i++)
    {
        Mat gP = giris(parcalar[i]);
        Mat cP = cikis(parcalar[i]);
        threadler.emplace_back([gP, cP]() mutable { donustur(gP, cP); });
        SetThreadAffinityMask(threadler.back().native_handle(), (DWORD_PTR)1 << i);
    }
    for (auto& t : threadler) t.join();
}

int main()
{
    // OpenCV'nin INFO mesajlarini kapat
    utils::logging::setLogLevel(utils::logging::LOG_LEVEL_ERROR);

    Mat kaynak = imread("resim.jpg");
    if (kaynak.empty())
    {
        cout << "resim.jpg bulunamadi!" << endl;
        return -1;
    }
    cout << "Donusum: yeni = " << ALFA << " * eski + " << BETA << endl;

    // 4 cekirdekle donustur
    Mat sonuc(kaynak.size(), kaynak.type());
    auto t1 = chrono::high_resolution_clock::now();
    dortCekirdekIsle(kaynak, sonuc);
    auto t2 = chrono::high_resolution_clock::now();
    cout << "4 cekirdek sure: "
        << chrono::duration<double, milli>(t2 - t1).count() << " ms" << endl;

    // Dogrulama: OpenCV'nin hazir fonksiyonu ayni isi yapar
    Mat opencvSonuc;
    kaynak.convertTo(opencvSonuc, -1, ALFA, BETA);
    double fark = norm(sonuc, opencvSonuc, NORM_INF);
    cout << "OpenCV convertTo ile en buyuk fark: " << fark
        << (fark <= 1 ? " (dogru)" : " (HATA)") << endl;

    // Ornek degerler
    cout << "\nOrnek: eski -> yeni" << endl;
    for (int v : { 0, 50, 100, 200, 255 })
        cout << v << " -> " << (int)saturate_cast<uchar>(ALFA * v + BETA) << endl;

    imshow("Orijinal", kaynak);
    imshow("0.75 * piksel + 20", sonuc);
    imwrite("odev4_sonuc.png", sonuc);

    waitKey(0);
    return 0;
}