// Hafta 4 - Odev 5: 3x3 Laplacian cekirdegi ile kenar tespiti
// Cekirdek: [[0 1 0], [1 -4 1], [0 1 0]]
// Gri goruntu once Gauss filtresiyle hafifce yumusatilir.
// Laplacian ikinci turevdir: kenar, cevabin isaret degistirdigi (sifirdan gectigi) yerdedir.
// Sonuclar Sobel ve Prewitt ile karsilastirilir (bkz. Karsilastirma.md).

#include "../Hafta4_Ortak/kenar_ortak.hpp"

struct Yontem
{
    const char* ad;
    function<Mat(const Mat&)> guc;   // yumusatilmis goruntuden kenar gucu haritasi
};

int main()
{
    // OpenCV'nin INFO mesajlarini kapat
    utils::logging::setLogLevel(utils::logging::LOG_LEVEL_ERROR);
    cout << fixed << setprecision(3);

    cout << "Laplacian cekirdegi:\n" << laplace3() << endl;

    Yontem yontemler[3] = {
        { "Prewitt",   [](const Mat& g) { return gradyanBuyuklugu(g, prewittX(), prewittY(), 3); } },
        { "Sobel",     [](const Mat& g) { return gradyanBuyuklugu(g, sobelX(), sobelY(), 4); } },
        { "Laplacian", [](const Mat& g) { return sifirGecisGucu(filtrele(g, laplace3())); } },
    };

    // --- Sentetik goruntu: dogruluk ve gurultu (en iyi esik burada bulunur) ---
    // Uc gurultu seviyesi denenir. Gercek goruntu icin esikler, gercek bir
    // fotografa en yakin olan dusuk gurultulu (sigma = 5) durumdan alinir.
    double esikler[3];
    for (double sigma : { 5.0, 12.0, 25.0 })
    {
        Mat temiz, gurultulu, gercek;
        sentetikGoruntu(temiz, gurultulu, gercek, sigma);
        Mat sYumusak = hafifYumusat(gurultulu);
        Mat uzak;
        dilate(gercek, uzak, getStructuringElement(MORPH_RECT, Size(9, 9)));
        uzak = ~uzak;

        cout << "\nSentetik goruntu (gurultu sigma = " << sigma << ", Gauss 3x3 ile yumusatilmis):" << endl;
        cout << "  Yontem      F1     Kesinlik  Duyarlilik  Duz bolgede yanlis kenar" << endl;
        for (int i = 0; i < 3; i++)
        {
            Mat g = yontemler[i].guc(sYumusak);
            double t;
            Dogruluk d = enIyiEsikle(g, gercek, &t);
            if (sigma == 5.0) esikler[i] = t;
            Mat kenar = esikle(g, t);
            int yanlis = countNonZero(kenar & uzak);
            cout << "  " << setw(10) << left << yontemler[i].ad << right << setw(7) << d.f1 << setw(10)
                << d.kesinlik << setw(12) << d.duyarlilik << setw(14) << yanlis << endl;
            if (sigma == 12.0) imwrite(string("sentetik_") + yontemler[i].ad + ".png", kenar);
        }

        // Laplacian'in gurultuye hassasiyeti: yumusatma OLMADAN
        Mat g = sifirGecisGucu(filtrele(gurultulu, laplace3()));
        double t;
        Dogruluk d = enIyiEsikle(g, gercek, &t);
        cout << "  Laplacian (yumusatmasiz) F1 = " << d.f1 << ", yanlis kenar = "
            << countNonZero(esikle(g, t) & uzak) << endl;
        if (sigma == 12.0) imwrite("sentetik_Laplacian_yumusatmasiz.png", esikle(g, t));
    }

    // --- Gercek goruntu ---
    Mat gri = griOku("resim.jpg");
    Mat yumusak = hafifYumusat(gri);
    cout << "\nGercek goruntu: " << gri.cols << " x " << gri.rows << endl;
    cout << "  Yontem      Kenar piksel orani   Sure (ms)" << endl;
    for (int i = 0; i < 3; i++)
    {
        Mat kenar = esikle(yontemler[i].guc(yumusak), esikler[i]);
        double ms = sureOlc([&]() { esikle(yontemler[i].guc(hafifYumusat(gri)), esikler[i]); });
        cout << "  " << setw(10) << left << yontemler[i].ad << right << setw(14)
            << 100.0 * countNonZero(kenar) / kenar.total() << " %" << setw(13) << ms << endl;
        imshow(yontemler[i].ad, kenar);
        imwrite(string("kenar_") + yontemler[i].ad + ".png", kenar);
    }

    // Laplacian'in ham cevabi (isaretli -> gri 128 sifir)
    Mat ham = filtrele(yumusak, laplace3()), hamG;
    ham.convertTo(hamG, CV_8U, 1, 128);
    imshow("Laplacian ham cevap (128 = 0)", hamG);
    imwrite("laplacian_ham.png", hamG);

    waitKey(0);
    return 0;
}
