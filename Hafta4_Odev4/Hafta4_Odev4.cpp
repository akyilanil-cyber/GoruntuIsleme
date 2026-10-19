// Hafta 4 - Odev 4: Sobel ve Prewitt operatorlerinin karsilastirilmasi
// Karsilastirma metnindeki (Karsilastirma.md) sayilar bu programla olculmustur.
// Olculenler:
//  1) Gercek goruntude cevap gucu ve iki sonucun benzerligi
//  2) Gurultu hassasiyeti (kenar olmayan duz bolgelerdeki cevap)
//  3) Bilinen kenarlara gore dogruluk (F1)
//  4) Gradyan yonunun dogrulugu (bir dairenin kenarinda)
//  5) Calisma suresi

#include "../Hafta4_Ortak/kenar_ortak.hpp"

// Duz bolgelerdeki (gercek kenardan en az 4 piksel uzak) ortalama cevap
double duzBolgeCevabi(const Mat& buyukluk, const Mat& gercek)
{
    Mat yakin;
    dilate(gercek, yakin, getStructuringElement(MORPH_RECT, Size(9, 9)));
    return mean(buyukluk, ~yakin)[0];
}

// Daire kenarinda olculen gradyan yonu ile gercek yon arasindaki ortalama aci hatasi (derece)
double yonHatasi(const Mat& kx, const Mat& ky)
{
    const int N = 401, R = 150;
    Mat daire(N, N, CV_8U, Scalar(60));
    circle(daire, Point(N / 2, N / 2), R, Scalar(200), FILLED, LINE_AA);
    Mat yumusak = hafifYumusat(daire);
    Mat gx = filtrele(yumusak, kx), gy = filtrele(yumusak, ky);

    double toplam = 0;
    int sayi = 0;
    for (int y = 0; y < N; y++)
        for (int x = 0; x < N; x++)
        {
            double dx = x - N / 2, dy = y - N / 2, r = sqrt(dx * dx + dy * dy);
            if (fabs(r - R) > 1) continue;
            double gercekAci = atan2(-dy, -dx);   // disaridan iceri (koyudan aciga)
            double olculen = atan2(gy.at<float>(y, x), gx.at<float>(y, x));
            double fark = fabs(gercekAci - olculen);
            if (fark > CV_PI) fark = 2 * CV_PI - fark;
            toplam += fark;
            sayi++;
        }
    return toplam / sayi * 180 / CV_PI;
}

int main()
{
    // OpenCV'nin INFO mesajlarini kapat
    utils::logging::setLogLevel(utils::logging::LOG_LEVEL_ERROR);
    cout << fixed << setprecision(3);

    struct Op { const char* ad; Mat kx, ky; float olcek; };
    Op ops[2] = { { "Prewitt", prewittX(), prewittY(), 3 }, { "Sobel", sobelX(), sobelY(), 4 } };

    // 1) Gercek goruntu
    Mat gri = griOku("resim.jpg");
    Mat yumusak = hafifYumusat(gri);
    cout << "Gercek goruntu: " << gri.cols << " x " << gri.rows << endl;

    Mat b[2];
    for (int i = 0; i < 2; i++)
    {
        b[i] = gradyanBuyuklugu(yumusak, ops[i].kx, ops[i].ky, ops[i].olcek);
        Mat g; b[i].convertTo(g, CV_8U);
        imwrite(string(ops[i].ad) + "_buyukluk.png", g);
        cout << "  " << setw(8) << left << ops[i].ad << right << " ortalama buyukluk = " << mean(b[i])[0]
            << ", kenar piksel (>40) = " << countNonZero(b[i] > 40) << endl;
    }
    Mat fark = abs(b[0] - b[1]);
    double enBuyukFark;
    minMaxLoc(fark, nullptr, &enBuyukFark);
    Mat kor;
    matchTemplate(b[0], b[1], kor, TM_CCORR_NORMED);
    cout << "  Iki sonuc arasinda ortalama |fark| = " << mean(fark)[0]
        << ", en buyuk = " << enBuyukFark << ", benzerlik (normalize korelasyon) = "
        << kor.at<float>(0, 0) << endl;
    Mat farkG;
    normalize(fark, farkG, 0, 255, NORM_MINMAX, CV_8U);
    imwrite("fark.png", farkG);

    // 2-3) Sentetik goruntu: gurultu ve dogruluk
    for (double sigma : { 12.0, 25.0 })
    {
        Mat temiz, gurultulu, gercek;
        sentetikGoruntu(temiz, gurultulu, gercek, sigma);
        imwrite("sentetik_gurultulu_" + to_string((int)sigma) + ".png", gurultulu);
        Mat sYumusak = hafifYumusat(gurultulu);

        cout << "\nSentetik goruntu (gurultu sigma = " << sigma << ", Gauss ile yumusatilmis):" << endl;
        cout << "  Yontem    Duz bolge cevabi   Kenar cevabi   Oran    F1     Kesinlik  Duyarlilik" << endl;
        for (int i = 0; i < 2; i++)
        {
            Mat bs = gradyanBuyuklugu(sYumusak, ops[i].kx, ops[i].ky, ops[i].olcek);
            double duz = duzBolgeCevabi(bs, gercek);
            double kenar = mean(bs, gercek)[0];
            Dogruluk d = enIyiEsikle(bs, gercek);
            cout << "  " << setw(8) << left << ops[i].ad << right << setw(14) << duz << setw(16) << kenar
                << setw(10) << kenar / duz << setw(8) << d.f1 << setw(9) << d.kesinlik << setw(11) << d.duyarlilik << endl;
        }
    }

    // 4) Yon dogrulugu
    cout << "\nDaire kenarinda ortalama yon hatasi:" << endl;
    for (int i = 0; i < 2; i++)
        cout << "  " << setw(8) << left << ops[i].ad << right << " " << yonHatasi(ops[i].kx, ops[i].ky) << " derece" << endl;

    // 5) Sure (yumusatma dahil)
    cout << "\nCalisma suresi (yumusatma + iki yon + buyukluk, " << gri.cols << "x" << gri.rows << "):" << endl;
    for (int i = 0; i < 2; i++)
    {
        double ms = sureOlc([&]() { gradyanBuyuklugu(hafifYumusat(gri), ops[i].kx, ops[i].ky, ops[i].olcek); });
        cout << "  " << setw(8) << left << ops[i].ad << right << " " << ms << " ms" << endl;
    }

    return 0;
}
