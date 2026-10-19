// Hafta 4 - Odev 6: 7x7 Laplacian of Gaussian (LoG) ile kenar tespiti
// LoG cekirdegi formulden uretilir (sigma = 1.0, toplami 0).
// Sonuc, Gauss filtresinden gecirilmis ve gecirilmemis diger kenar filtreleriyle
// (Prewitt, Sobel, Laplacian) karsilastirilir (bkz. Rapor.md).

#include "../Hafta4_Ortak/kenar_ortak.hpp"

struct Yontem
{
    string ad;
    function<Mat(const Mat&)> guc;   // ham gri goruntuden kenar gucu haritasi
};

int main()
{
    // OpenCV'nin INFO mesajlarini kapat
    utils::logging::setLogLevel(utils::logging::LOG_LEVEL_ERROR);
    cout << fixed << setprecision(3);

    Mat log7 = logCekirdegi(7, 1.0);
    cout << "7x7 LoG cekirdegi (sigma = 1.0, x1000):\n";
    for (int y = 0; y < 7; y++)
    {
        for (int x = 0; x < 7; x++) cout << setw(8) << setprecision(1) << log7.at<float>(y, x) * 1000;
        cout << endl;
    }
    cout << setprecision(3) << "Cekirdek toplami: " << sum(log7)[0] << endl;

    vector<Yontem> yontemler = {
        { "Prewitt",              [](const Mat& g) { return gradyanBuyuklugu(g, prewittX(), prewittY(), 3); } },
        { "Gauss + Prewitt",      [](const Mat& g) { return gradyanBuyuklugu(hafifYumusat(g), prewittX(), prewittY(), 3); } },
        { "Sobel",                [](const Mat& g) { return gradyanBuyuklugu(g, sobelX(), sobelY(), 4); } },
        { "Gauss + Sobel",        [](const Mat& g) { return gradyanBuyuklugu(hafifYumusat(g), sobelX(), sobelY(), 4); } },
        { "Laplacian",            [](const Mat& g) { return sifirGecisGucu(filtrele(g, laplace3())); } },
        { "Gauss + Laplacian",    [](const Mat& g) { return sifirGecisGucu(filtrele(hafifYumusat(g), laplace3())); } },
        { "LoG 7x7",              [&](const Mat& g) { return sifirGecisGucu(filtrele(g, log7)); } },
    };

    // --- Sentetik goruntu: uc gurultu seviyesinde dogruluk ---
    // Gercek goruntu icin esikler dusuk gurultulu (sigma = 5) durumdan alinir.
    vector<double> esikler(yontemler.size());
    for (double sigma : { 5.0, 12.0, 25.0 })
    {
        Mat temiz, gurultulu, gercek;
        sentetikGoruntu(temiz, gurultulu, gercek, sigma);
        Mat uzak;
        dilate(gercek, uzak, getStructuringElement(MORPH_RECT, Size(9, 9)));
        uzak = ~uzak;

        cout << "\nSentetik goruntu, gurultu sigma = " << sigma << endl;
        cout << "  Yontem                F1     Kesinlik  Duyarlilik  Yanlis kenar" << endl;
        for (size_t i = 0; i < yontemler.size(); i++)
        {
            Mat g = yontemler[i].guc(gurultulu);
            double t;
            Dogruluk d = enIyiEsikle(g, gercek, &t);
            int yanlis = countNonZero(esikle(g, t) & uzak);
            cout << "  " << setw(20) << left << yontemler[i].ad << right << setw(7) << d.f1 << setw(10)
                << d.kesinlik << setw(12) << d.duyarlilik << setw(13) << yanlis << endl;
            if (sigma == 5.0) esikler[i] = t;
            if (sigma == 12.0)
            {
                string dosya = dosyaAdi(yontemler[i].ad);
                imwrite("sentetik_" + dosya + ".png", esikle(g, t));
            }
        }
    }

    // --- Gercek goruntu ---
    Mat gri = griOku("resim.jpg");
    cout << "\nGercek goruntu: " << gri.cols << " x " << gri.rows << endl;
    cout << "  Yontem                Kenar piksel orani   Sure (ms)" << endl;
    for (size_t i = 0; i < yontemler.size(); i++)
    {
        Mat kenar = esikle(yontemler[i].guc(gri), esikler[i]);
        double ms = sureOlc([&]() { esikle(yontemler[i].guc(gri), esikler[i]); });
        cout << "  " << setw(20) << left << yontemler[i].ad << right << setw(14)
            << 100.0 * countNonZero(kenar) / kenar.total() << " %" << setw(13) << ms << endl;

        string dosya = dosyaAdi(yontemler[i].ad);
        imwrite("kenar_" + dosya + ".png", kenar);
        if (yontemler[i].ad == "LoG 7x7") imshow("LoG 7x7 kenarlar", kenar);
    }

    // Cekirdegin gorunumu (buyutulmus)
    Mat cg;
    normalize(log7, cg, 0, 255, NORM_MINMAX, CV_8U);
    resize(cg, cg, Size(210, 210), 0, 0, INTER_NEAREST);
    imwrite("log_cekirdek.png", cg);

    imshow("Gri", gri);
    waitKey(0);
    return 0;
}
