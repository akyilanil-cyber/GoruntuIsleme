// Hafta 3 - Gorev 3: CLAHE (Contrast Limited Adaptive Histogram Equalization)
// 1) OpenCV'nin hazir CLAHE fonksiyonu ile resim duzeltilir.
// 2) Ayni algoritma HAZIR FONKSIYON KULLANILMADAN yazilir.
// 3) Iki sonuc karsilastirilarak test edilir.

#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
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
        if (kucuk.find("clahe") != string::npos) continue; // kendi ciktilarimizi okumayalim

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
        cout << "Klasorde okunabilir bir resim bulunamadi! Resmi .cpp dosyasinin yanina koy." << endl;
    return sonuc;
}

// Kenarin disina tasan indeksi aynalar (OpenCV'deki BORDER_REFLECT_101 ile ayni)
int aynala(int i, int n)
{
    if (i < 0) return -i;
    if (i >= n) return 2 * n - 2 - i;
    return i;
}

// Elle CLAHE
// kirpmaSiniri: clip limit (ornegin 2.0), karoX/karoY: karo (tile) sayisi (ornegin 8x8)
Mat claheManuel(const Mat& src, double kirpmaSiniri, int karoX, int karoY)
{
    const int H = 256;

    // Resim karo sayisina tam bolunmuyorsa, LUT hesabi icin sanal olarak genisletilir
    int genW = src.cols, genH = src.rows;
    if (src.cols % karoX != 0 || src.rows % karoY != 0)
    {
        genW = src.cols + karoX - (src.cols % karoX);
        genH = src.rows + karoY - (src.rows % karoY);
    }
    int karoW = genW / karoX;
    int karoH = genH / karoY;
    int karoAlan = karoW * karoH;

    // Kirpma siniri: her degerin bir karo icinde en fazla kac kez sayilabilecegi
    int sinir = (int)(kirpmaSiniri * karoAlan / H);
    if (sinir < 1) sinir = 1;
    float olcek = (float)(H - 1) / karoAlan;

    // 1) Her karo icin: histogram -> kirp -> fazlayi dagit -> CDF -> donusum tablosu
    vector<vector<uchar>> lut(karoX * karoY, vector<uchar>(H));
    for (int ty = 0; ty < karoY; ty++)
    {
        for (int tx = 0; tx < karoX; tx++)
        {
            int hist[H] = { 0 };
            for (int yy = 0; yy < karoH; yy++)
            {
                int y = aynala(ty * karoH + yy, src.rows);
                const uchar* satir = src.ptr<uchar>(y);
                for (int xx = 0; xx < karoW; xx++)
                    hist[satir[aynala(tx * karoW + xx, src.cols)]]++;
            }

            // Kirpma: sinirin ustundeki kisim kesilir
            int kesilen = 0;
            for (int i = 0; i < H; i++)
            {
                if (hist[i] > sinir)
                {
                    kesilen += hist[i] - sinir;
                    hist[i] = sinir;
                }
            }

            // Kesilen kisim tum degerlere esit dagitilir
            int esitPay = kesilen / H;
            int artan = kesilen - esitPay * H;
            for (int i = 0; i < H; i++) hist[i] += esitPay;
            if (artan != 0)
            {
                int adim = max(H / artan, 1);
                for (int i = 0; i < H && artan > 0; i += adim, artan--)
                    hist[i]++;
            }

            // CDF -> donusum tablosu
            int toplam = 0;
            vector<uchar>& tablo = lut[ty * karoX + tx];
            for (int i = 0; i < H; i++)
            {
                toplam += hist[i];
                tablo[i] = saturate_cast<uchar>(toplam * olcek);
            }
        }
    }

    // 2) Her piksel, en yakin 4 karonun tablosu arasinda bilineer interpolasyonla donusturulur
    //    (karo sinirlarinda bloklasma olmasin diye)
    Mat dst(src.size(), CV_8UC1);
    float tersW = 1.0f / karoW, tersH = 1.0f / karoH;

    for (int y = 0; y < src.rows; y++)
    {
        float tyf = y * tersH - 0.5f;
        int ty1 = (int)floor(tyf);
        int ty2 = ty1 + 1;
        float ya = tyf - ty1;
        ty1 = max(ty1, 0);
        ty2 = min(ty2, karoY - 1);

        const uchar* s = src.ptr<uchar>(y);
        uchar* d = dst.ptr<uchar>(y);

        for (int x = 0; x < src.cols; x++)
        {
            float txf = x * tersW - 0.5f;
            int tx1 = (int)floor(txf);
            int tx2 = tx1 + 1;
            float xa = txf - tx1;
            tx1 = max(tx1, 0);
            tx2 = min(tx2, karoX - 1);

            uchar v = s[x];
            float ust = lut[ty1 * karoX + tx1][v] * (1.0f - xa) + lut[ty1 * karoX + tx2][v] * xa;
            float alt = lut[ty2 * karoX + tx1][v] * (1.0f - xa) + lut[ty2 * karoX + tx2][v] * xa;
            d[x] = saturate_cast<uchar>(ust * (1.0f - ya) + alt * ya);
        }
    }
    return dst;
}

int main()
{
    utils::logging::setLogLevel(utils::logging::LOG_LEVEL_ERROR);

    Mat gri = griResimAc();
    if (gri.empty()) return -1;
    cout << "Goruntu: " << gri.cols << " x " << gri.rows << endl;

    const double KIRPMA = 2.0;
    const int KARO = 8; // 8x8 karo

    // 1) OpenCV'nin hazir CLAHE fonksiyonu
    Ptr<CLAHE> clahe = createCLAHE(KIRPMA, Size(KARO, KARO));
    Mat hazir;
    clahe->apply(gri, hazir);

    // 2) Elle yazilan CLAHE
    Mat manuel = claheManuel(gri, KIRPMA, KARO, KARO);

    // 3) Test: iki sonucu karsilastir
    Mat fark;
    absdiff(hazir, manuel, fark);
    double enKucuk, enBuyuk;
    minMaxLoc(fark, &enKucuk, &enBuyuk);
    double ortalamaFark = mean(fark)[0];
    int farkliPiksel = countNonZero(fark);

    cout << "\n--- Test: Hazir CLAHE vs Elle CLAHE ---" << endl;
    cout << "En buyuk piksel farki : " << enBuyuk << endl;
    cout << "Ortalama piksel farki : " << ortalamaFark << endl;
    cout << "Farkli piksel sayisi  : " << farkliPiksel << " / " << gri.total() << endl;
    cout << (enBuyuk <= 1 ? "SONUC: Elle yazilan CLAHE, OpenCV ile ayni sonucu veriyor."
        : "SONUC: Sonuclar arasinda fark var.") << endl;

    imwrite("clahe_hazir.png", hazir);
    imwrite("clahe_manuel.png", manuel);

    imshow("Orijinal", gri);
    imshow("CLAHE - OpenCV hazir", hazir);
    imshow("CLAHE - Elle yazilan", manuel);

    waitKey(0);
    return 0;
}