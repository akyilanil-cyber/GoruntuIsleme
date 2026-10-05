// Hafta 3 - Gorev 4: 3x3 Ortalama Filtresi ile Gurultu Bastirma (Konvolusyon)
// Gurultulu bir resim uzerinde 3x3 ortalama filtresi elle konvolusyonla gezdirilir.
// Klasorde adinda "gurultulu" gecen bir resim varsa o kullanilir;
// yoksa klasordeki resme yapay Gauss gurultusu eklenir.

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

// Dosyayi icerigine bakarak okur (uzanti yanlis olsa bile calisir)
Mat dosyadanOku(const string& ad)
{
    ifstream dosya(ad, ios::binary);
    vector<uchar> veri((istreambuf_iterator<char>(dosya)), istreambuf_iterator<char>());
    if (veri.empty()) return Mat();
    return imdecode(veri, IMREAD_COLOR);
}

// Klasordeki resimleri bulur: adinda "gurultulu" gecen varsa onu, yoksa ilk resmi dondurur
Mat resimBul(bool& gurultuluMu)
{
    vector<string> uzantilar = { ".jpg", ".jpeg", ".png", ".bmp", ".webp" };
    string gurultuluAd, normalAd;

    _finddata_t bilgi;
    intptr_t h = _findfirst("*.*", &bilgi);
    if (h != -1)
    {
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
            if (kucuk.find("filtreli") != string::npos || kucuk.find("uretilen") != string::npos) continue;

            if (kucuk.find("gurultulu") != string::npos) { if (gurultuluAd.empty()) gurultuluAd = ad; }
            else if (normalAd.empty()) normalAd = ad;
        } while (_findnext(h, &bilgi) == 0);
        _findclose(h);
    }

    if (!gurultuluAd.empty())
    {
        Mat m = dosyadanOku(gurultuluAd);
        if (!m.empty()) { cout << "Gurultulu resim: " << gurultuluAd << endl; gurultuluMu = true; return m; }
    }
    if (!normalAd.empty())
    {
        Mat m = dosyadanOku(normalAd);
        if (!m.empty()) { cout << "Kullanilan resim: " << normalAd << endl; gurultuluMu = false; return m; }
    }
    cout << "Klasorde okunabilir bir resim bulunamadi! Resmi .cpp dosyasinin yanina koy." << endl;
    return Mat();
}

// Kenarin disina tasan indeksi aynalar (OpenCV'deki BORDER_REFLECT_101 ile ayni)
int aynala(int i, int n)
{
    if (i < 0) return -i;
    if (i >= n) return 2 * n - 2 - i;
    return i;
}

// 3x3 ortalama filtresi ile konvolusyon (gri veya renkli resim)
Mat ortalamaFiltresi(const Mat& src)
{
    // Cekirdek (kernel): 9 elemanin hepsi 1/9 -> 3x3 komsulugun ortalamasi
    const float K[3][3] = {
        { 1 / 9.0f, 1 / 9.0f, 1 / 9.0f },
        { 1 / 9.0f, 1 / 9.0f, 1 / 9.0f },
        { 1 / 9.0f, 1 / 9.0f, 1 / 9.0f }
    };

    int kanal = src.channels();
    Mat dst(src.size(), src.type());

    for (int y = 0; y < src.rows; y++)
    {
        uchar* d = dst.ptr<uchar>(y);
        for (int x = 0; x < src.cols; x++)
        {
            for (int c = 0; c < kanal; c++)
            {
                float toplam = 0;
                // Cekirdegi pikselin uzerine koy, karsilikli carp ve topla
                for (int ky = -1; ky <= 1; ky++)
                {
                    const uchar* s = src.ptr<uchar>(aynala(y + ky, src.rows));
                    for (int kx = -1; kx <= 1; kx++)
                    {
                        int xx = aynala(x + kx, src.cols);
                        toplam += K[ky + 1][kx + 1] * s[xx * kanal + c];
                    }
                }
                d[x * kanal + c] = saturate_cast<uchar>(toplam);
            }
        }
    }
    return dst;
}

int main()
{
    utils::logging::setLogLevel(utils::logging::LOG_LEVEL_ERROR);

    bool gurultuluMu = false;
    Mat okunan = resimBul(gurultuluMu);
    if (okunan.empty()) return -1;

    Mat temiz, gurultulu;
    if (gurultuluMu)
    {
        gurultulu = okunan;
    }
    else
    {
        temiz = okunan;
        cout << "Gurultulu resim yok, Gauss gurultusu ekleniyor (sigma = 25)" << endl;
        theRNG().state = 12345; // her calistirmada ayni gurultu
        Mat temizF, gurultu;
        temiz.convertTo(temizF, CV_32F);
        gurultu.create(temiz.size(), CV_32FC(temiz.channels()));
        randn(gurultu, 0, 25);
        Mat toplamF = temizF + gurultu;
        toplamF.convertTo(gurultulu, CV_8U);
        imwrite("gurultulu_uretilen.png", gurultulu);
    }

    cout << "Goruntu: " << gurultulu.cols << " x " << gurultulu.rows
        << ", kanal: " << gurultulu.channels() << endl;

    // Elle konvolusyon
    Mat filtreli = ortalamaFiltresi(gurultulu);

    // Test: OpenCV'nin blur fonksiyonu ile karsilastir (sadece dogrulama icin)
    Mat opencvBlur;
    blur(gurultulu, opencvBlur, Size(3, 3));
    Mat fark;
    absdiff(filtreli, opencvBlur, fark);
    double enKucuk, enBuyuk;
    minMaxLoc(fark.reshape(1), &enKucuk, &enBuyuk);
    cout << "OpenCV blur ile en buyuk fark: " << enBuyuk
        << (enBuyuk <= 1 ? " (dogru)" : " (HATA)") << endl;

    // Gurultu olcumu: PSNR yukseldikce resim temize yaklasir
    if (!temiz.empty())
    {
        cout << "PSNR gurultulu : " << PSNR(temiz, gurultulu) << " dB" << endl;
        cout << "PSNR filtreli  : " << PSNR(temiz, filtreli) << " dB" << endl;
    }

    imwrite("filtreli.png", filtreli);

    if (!temiz.empty()) imshow("Temiz (orijinal)", temiz);
    imshow("Gurultulu", gurultulu);
    imshow("3x3 ortalama filtresi", filtreli);

    waitKey(0);
    return 0;
}