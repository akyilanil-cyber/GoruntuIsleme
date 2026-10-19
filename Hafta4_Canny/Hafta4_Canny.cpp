// Hafta 4 - Haftalik Odev: Canny kenar bulma algoritmasinin sifirdan yazilmasi
// Adimlarin her biri ayri bir fonksiyondur:
//   1) gurultuBastir       : 5x5 Gauss filtresi
//   2) sobelGradyan        : Sobel ile gradyan buyuklugu ve yonu
//   3) nonMaxBastirma      : Non-maximum suppression (kenarlari 1 piksele inceltme)
//   4) histerezisEsikleme  : Cift esik + baglantili bilesen takibi
// Konvolusyon dahil hicbir adimda OpenCV'nin hazir filtreleri kullanilmaz.
// Sonuc en sonda OpenCV'nin Canny fonksiyonuyla karsilastirilir.

#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>
#include "../Hafta4_Ortak/kenar_ortak.hpp"   // sentetik goruntu ve F1 olcumu (adim deneyi icin)

using namespace cv;
using namespace std;

// ---------------------------------------------------------------------------
// Yardimci: elle konvolusyon (kenarlarda goruntu aynalanir)
// ---------------------------------------------------------------------------
int aynala(int i, int n)
{
    if (i < 0) return -i;
    if (i >= n) return 2 * n - i - 2;
    return i;
}

Mat konvolusyon(const Mat& giris, const Mat& cekirdek)
{
    CV_Assert(giris.type() == CV_32F && cekirdek.type() == CV_32F);
    int m = cekirdek.rows / 2;
    Mat cikis(giris.size(), CV_32F);
    for (int y = 0; y < giris.rows; y++)
        for (int x = 0; x < giris.cols; x++)
        {
            float toplam = 0;
            for (int ky = -m; ky <= m; ky++)
            {
                const float* satir = giris.ptr<float>(aynala(y + ky, giris.rows));
                for (int kx = -m; kx <= m; kx++)
                    toplam += cekirdek.at<float>(ky + m, kx + m) * satir[aynala(x + kx, giris.cols)];
            }
            cikis.at<float>(y, x) = toplam;
        }
    return cikis;
}

// ---------------------------------------------------------------------------
// ADIM 1: Gurultu bastirma - 5x5 Gauss filtresi
// G(x,y) = exp(-(x^2 + y^2) / (2 sigma^2)), toplami 1 olacak sekilde normalize edilir.
// ---------------------------------------------------------------------------
Mat gaussCekirdegi5x5(double sigma)
{
    Mat k(5, 5, CV_32F);
    for (int y = -2; y <= 2; y++)
        for (int x = -2; x <= 2; x++)
            k.at<float>(y + 2, x + 2) = (float)exp(-(x * x + y * y) / (2 * sigma * sigma));
    return k / sum(k)[0];
}

Mat gurultuBastir(const Mat& gri, double sigma = 1.4)
{
    Mat f;
    gri.convertTo(f, CV_32F);
    return konvolusyon(f, gaussCekirdegi5x5(sigma));
}

// ---------------------------------------------------------------------------
// ADIM 2: Sobel ile gradyan
// buyukluk = sqrt(gx^2 + gy^2), yon = atan2(gy, gx) (derece, 0-180)
// ---------------------------------------------------------------------------
void sobelGradyan(const Mat& goruntu, Mat& buyukluk, Mat& yon)
{
    float dx[9] = { -1, 0, 1, -2, 0, 2, -1, 0, 1 };
    float dy[9] = { -1, -2, -1, 0, 0, 0, 1, 2, 1 };
    Mat kx(3, 3, CV_32F, dx), ky(3, 3, CV_32F, dy);
    Mat gx = konvolusyon(goruntu, kx);
    Mat gy = konvolusyon(goruntu, ky);

    buyukluk.create(goruntu.size(), CV_32F);
    yon.create(goruntu.size(), CV_32F);
    for (int y = 0; y < goruntu.rows; y++)
        for (int x = 0; x < goruntu.cols; x++)
        {
            float a = gx.at<float>(y, x), b = gy.at<float>(y, x);
            buyukluk.at<float>(y, x) = sqrt(a * a + b * b);
            float d = (float)(atan2(b, a) * 180 / CV_PI);   // -180..180
            if (d < 0) d += 180;                            // yon (isaret onemsiz): 0..180
            yon.at<float>(y, x) = d;
        }
}

// ---------------------------------------------------------------------------
// ADIM 3: Non-maximum suppression
// Her piksel, gradyan yonundeki iki komsusuyla karsilastirilir. Yerel en buyuk
// degilse silinir. Yon 4 sinifa yuvarlanir: 0, 45, 90, 135 derece.
// ---------------------------------------------------------------------------
Mat nonMaxBastirma(const Mat& buyukluk, const Mat& yon)
{
    Mat cikis = Mat::zeros(buyukluk.size(), CV_32F);
    for (int y = 1; y < buyukluk.rows - 1; y++)
        for (int x = 1; x < buyukluk.cols - 1; x++)
        {
            float m = buyukluk.at<float>(y, x);
            if (m == 0) continue;
            float d = yon.at<float>(y, x);
            float once, sonra;   // gradyan yonundeki iki komsu
            if (d < 22.5 || d >= 157.5)      // yatay gradyan -> sol / sag
            {
                once = buyukluk.at<float>(y, x - 1);  sonra = buyukluk.at<float>(y, x + 1);
            }
            else if (d < 67.5)               // 45 derece -> sol ust / sag alt
            {
                once = buyukluk.at<float>(y - 1, x - 1);  sonra = buyukluk.at<float>(y + 1, x + 1);
            }
            else if (d < 112.5)              // dikey gradyan -> ust / alt
            {
                once = buyukluk.at<float>(y - 1, x);  sonra = buyukluk.at<float>(y + 1, x);
            }
            else                             // 135 derece -> sag ust / sol alt
            {
                once = buyukluk.at<float>(y - 1, x + 1);  sonra = buyukluk.at<float>(y + 1, x - 1);
            }
            // Esitlik durumunda cift kalin kenar olusmamasi icin bir tarafta >, digerinde >=
            if (m > once && m >= sonra)
                cikis.at<float>(y, x) = m;
        }
    return cikis;
}

// ---------------------------------------------------------------------------
// ADIM 4: Histerezis esikleme
// buyukluk >= yuksek  -> guclu kenar (kesin kenar)
// dusuk <= b < yuksek -> zayif kenar (ancak bir guclu kenara bagliysa kenar)
// buyukluk < dusuk    -> kenar degil
// Guclu kenarlardan baslayip 8 komsuluk ile zayif kenarlar takip edilir.
// ---------------------------------------------------------------------------
Mat histerezisEsikleme(const Mat& nms, float dusuk, float yuksek)
{
    Mat kenar = Mat::zeros(nms.size(), CV_8U);
    vector<Point> yigin;

    for (int y = 0; y < nms.rows; y++)
        for (int x = 0; x < nms.cols; x++)
            if (nms.at<float>(y, x) >= yuksek)
            {
                kenar.at<uchar>(y, x) = 255;
                yigin.push_back(Point(x, y));
            }

    while (!yigin.empty())
    {
        Point p = yigin.back();
        yigin.pop_back();
        for (int dy = -1; dy <= 1; dy++)
            for (int dx = -1; dx <= 1; dx++)
            {
                int nx = p.x + dx, ny = p.y + dy;
                if (nx < 0 || ny < 0 || nx >= nms.cols || ny >= nms.rows) continue;
                if (kenar.at<uchar>(ny, nx) == 0 && nms.at<float>(ny, nx) >= dusuk)
                {
                    kenar.at<uchar>(ny, nx) = 255;   // zayif kenar, guclu kenara bagli
                    yigin.push_back(Point(nx, ny));
                }
            }
    }
    return kenar;
}

// Tum adimlar
Mat benimCanny(const Mat& gri, float dusuk, float yuksek, Mat* ara[3] = nullptr)
{
    Mat yumusak = gurultuBastir(gri);
    Mat buyukluk, yon;
    sobelGradyan(yumusak, buyukluk, yon);
    Mat nms = nonMaxBastirma(buyukluk, yon);
    Mat kenar = histerezisEsikleme(nms, dusuk, yuksek);
    if (ara) { *ara[0] = yumusak; *ara[1] = buyukluk; *ara[2] = nms; }
    return kenar;
}

// 1 piksel toleransla iki kenar haritasinin uyumu (F1)
double uyum(const Mat& a, const Mat& b)
{
    Mat el = getStructuringElement(MORPH_RECT, Size(3, 3)), aG, bG;
    dilate(a, aG, el);
    dilate(b, bG, el);
    double k = (double)countNonZero(a & bG) / max(1, countNonZero(a));
    double d = (double)countNonZero(b & aG) / max(1, countNonZero(b));
    return 2 * k * d / (k + d);
}

// ---------------------------------------------------------------------------
// ADIM DENEYI: Her adim neden gerekli?
// Kenarlarin yeri bilinen sentetik goruntude adimlar tek tek cikarilir.
// Her surum icin F1'i en yuksek yapan esik aranir (dusuk = yuksek / 2),
// boylece fark esik seciminden degil adimin kendisinden kaynaklanir.
// ---------------------------------------------------------------------------
struct DeneySonucu { Dogruluk d; double kalinlik; int parca; };

DeneySonucu deneyOlc(const Mat& kenar, const Mat& gercek)
{
    DeneySonucu s;
    s.d = dogrulukOlc(kenar, gercek);
    // Gercek kenar (morfolojik gradyan) 2 piksel kalinligindadir; uzunlugu = sayi / 2
    s.kalinlik = countNonZero(kenar) / (countNonZero(gercek) / 2.0);
    Mat etiket;
    s.parca = connectedComponents(kenar, etiket, 8) - 1;   // arka plan haric parca sayisi
    return s;
}

void adimDeneyi(double gurultuSigma)
{
    Mat temiz, gurultulu, gercek;
    sentetikGoruntu(temiz, gurultulu, gercek, gurultuSigma);
    int gercekParca;
    { Mat e; gercekParca = connectedComponents(gercek, e, 8) - 1; }

    // Ara sonuclar bir kez hesaplanir, esik aramasi sadece son adimi tekrarlar
    Mat f; gurultulu.convertTo(f, CV_32F);
    Mat yumusak = gurultuBastir(gurultulu);
    Mat bG, yG, bY, yY;
    sobelGradyan(yumusak, bG, yG);    // Gauss var
    sobelGradyan(f, bY, yY);          // Gauss yok
    Mat nmsG = nonMaxBastirma(bG, yG);
    Mat nmsY = nonMaxBastirma(bY, yY);

    auto tekEsik = [](const Mat& m, double t) { Mat k = m >= t; return k; };
    struct Surum { const char* ad; const char* dosya; function<Mat(double)> uret; double ust; };
    double mG, mY;
    minMaxLoc(bG, nullptr, &mG);
    minMaxLoc(bY, nullptr, &mY);
    vector<Surum> surumler = {
        { "Tam Canny (4 adim)        ", "deney_tam_canny.png",   [&](double t) { return histerezisEsikleme(nmsG, (float)t / 2, (float)t); }, mG },
        { "1. adim yok (Gauss yok)   ", "deney_gauss_yok.png",   [&](double t) { return histerezisEsikleme(nmsY, (float)t / 2, (float)t); }, mY },
        { "3. adim yok (NMS yok)     ", "deney_nms_yok.png",     [&](double t) { return histerezisEsikleme(bG, (float)t / 2, (float)t); }, mG },
        { "4. adim yok (tek esik)    ", "deney_tek_esik.png",    [&](double t) { return tekEsik(nmsG, t); }, mG },
        { "Sadece Gauss+Sobel+esik   ", "deney_sadece_sobel.png", [&](double t) { return tekEsik(bG, t); }, mG },
    };

    cout << "\nGurultu sigma = " << gurultuSigma << " (gercek kenar parca sayisi = " << gercekParca << ")" << endl;
    cout << "  Surum                       F1     Kesinlik Duyarlilik Kalinlik Parca" << endl;
    for (const Surum& s : surumler)
    {
        double esik;
        enIyiEsikAra(s.uret, s.ust, gercek, &esik);
        Mat kenar = s.uret(esik);
        DeneySonucu r = deneyOlc(kenar, gercek);
        cout << "  " << s.ad << "  " << r.d.f1 << "  " << r.d.kesinlik << "    " << r.d.duyarlilik
            << "      " << setprecision(2) << r.kalinlik << " px  " << r.parca << setprecision(3) << endl;
        if (gurultuSigma == 40)
            imwrite(s.dosya, kenar);
    }
    if (gurultuSigma == 40)
        imwrite("deney_gurultulu_girdi.png", gurultulu);
}

int main()
{
    // OpenCV'nin INFO mesajlarini kapat
    utils::logging::setLogLevel(utils::logging::LOG_LEVEL_ERROR);
    cout << fixed << setprecision(3);

    Mat gri = imread("resim.jpg", IMREAD_GRAYSCALE);
    if (gri.empty())
    {
        cout << "resim.jpg bulunamadi!" << endl;
        return -1;
    }
    cout << "Goruntu: " << gri.cols << " x " << gri.rows << endl;

    const float DUSUK = 50, YUKSEK = 100;
    cout << "5x5 Gauss cekirdegi (sigma = 1.4):\n" << gaussCekirdegi5x5(1.4) << endl;
    cout << "Esikler: dusuk = " << DUSUK << ", yuksek = " << YUKSEK << endl;

    // Adim adim calistir ve ara sonuclari kaydet
    auto t1 = chrono::high_resolution_clock::now();
    Mat yumusak, buyukluk, nms;
    Mat* ara[3] = { &yumusak, &buyukluk, &nms };
    Mat benim = benimCanny(gri, DUSUK, YUKSEK, ara);
    auto t2 = chrono::high_resolution_clock::now();

    Mat g1, g2, g3;
    yumusak.convertTo(g1, CV_8U);
    normalize(buyukluk, g2, 0, 255, NORM_MINMAX, CV_8U);
    normalize(nms, g3, 0, 255, NORM_MINMAX, CV_8U);
    imwrite("1_gauss.png", g1);
    imwrite("2_sobel_gradyan.png", g2);
    imwrite("3_non_max_suppression.png", g3);
    imwrite("4_histerezis_sonuc.png", benim);

    cout << "\nAdimlar sonrasi aday kenar piksel sayisi:" << endl;
    cout << "  Gradyan > 0            : " << countNonZero(buyukluk > 0) << endl;
    cout << "  Non-max suppression    : " << countNonZero(nms > 0) << endl;
    cout << "  Guclu (>= " << YUKSEK << ")         : " << countNonZero(nms >= YUKSEK) << endl;
    cout << "  Zayif (" << DUSUK << "-" << YUKSEK << ")       : " << countNonZero((nms >= DUSUK) & (nms < YUKSEK)) << endl;
    cout << "  Histerezis sonrasi     : " << countNonZero(benim) << endl;

    // OpenCV Canny ile karsilastirma: ayni Gauss, ayni esikler, L2 gradyan
    Mat yumusak8, opencv;
    yumusak.convertTo(yumusak8, CV_8U);
    auto t3 = chrono::high_resolution_clock::now();
    Canny(yumusak8, opencv, DUSUK, YUKSEK, 3, true);
    auto t4 = chrono::high_resolution_clock::now();
    imwrite("opencv_canny.png", opencv);

    Mat birebir = benim == opencv;
    cout << "\nOpenCV Canny ile karsilastirma:" << endl;
    cout << "  Kenar piksel: kendi = " << countNonZero(benim) << ", OpenCV = " << countNonZero(opencv) << endl;
    cout << "  Birebir ayni piksel orani : %" << 100.0 * countNonZero(birebir) / birebir.total() << endl;
    cout << "  1 piksel toleransla uyum  : " << uyum(benim, opencv) << " (F1)" << endl;
    cout << "  Sure: kendi = " << chrono::duration<double, milli>(t2 - t1).count()
        << " ms, OpenCV = " << chrono::duration<double, milli>(t4 - t3).count() << " ms" << endl;

    // Farklari renkli goster: beyaz = ikisinde de, kirmizi = sadece kendi, yesil = sadece OpenCV
    Mat renkli(gri.size(), CV_8UC3, Scalar(0, 0, 0));
    renkli.setTo(Scalar(255, 255, 255), benim & opencv);
    renkli.setTo(Scalar(0, 0, 255), benim & ~opencv);
    renkli.setTo(Scalar(0, 255, 0), ~benim & opencv);
    imwrite("fark_renkli.png", renkli);

    // Her adim neden gerekli? Adimlari tek tek cikararak olc
    cout << "\n=== Adim deneyi (sentetik goruntu, 2 piksel tolerans) ===" << endl;
    for (double s : { 12.0, 25.0, 40.0 })
        adimDeneyi(s);

    imshow("Gri", gri);
    imshow("1) Gauss 5x5", g1);
    imshow("2) Sobel gradyan", g2);
    imshow("3) Non-max suppression", g3);
    imshow("4) Histerezis (sonuc)", benim);
    imshow("OpenCV Canny", opencv);
    waitKey(0);
    return 0;
}
