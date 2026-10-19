// Hafta 4 - Odev 7: OpenCV Canny ile kenar tespiti
// Canny sonucu, diger algoritmalarla (Prewitt, Sobel, Laplacian, LoG)
// calisma zamani ve dogruluk acisindan karsilastirilir.
// Dogruluk, kenarlarin yeri bilinen sentetik goruntude olculur (2 piksel tolerans, F1).
// Sure, gercek goruntude 30 tekrarin ortalamasidir (yumusatma dahil).

#include "../Hafta4_Ortak/kenar_ortak.hpp"

struct Yontem
{
    string ad;
    function<Mat(const Mat&, double)> kenar;   // gri goruntu + esik -> ikili kenar haritasi
    double esikUst;                             // esik aramasinin ust siniri
};

int main()
{
    // OpenCV'nin INFO mesajlarini kapat
    utils::logging::setLogLevel(utils::logging::LOG_LEVEL_ERROR);
    cout << fixed << setprecision(3);

    Mat log7 = logCekirdegi(7, 1.0);

    vector<Yontem> yontemler = {
        { "Gauss + Prewitt",   [](const Mat& g, double t) { return esikle(gradyanBuyuklugu(hafifYumusat(g), prewittX(), prewittY(), 3), t); }, 255 },
        { "Gauss + Sobel",     [](const Mat& g, double t) { return esikle(gradyanBuyuklugu(hafifYumusat(g), sobelX(), sobelY(), 4), t); }, 255 },
        { "Gauss + Laplacian", [](const Mat& g, double t) { return esikle(sifirGecisGucu(filtrele(hafifYumusat(g), laplace3())), t); }, 200 },
        { "LoG 7x7",           [&](const Mat& g, double t) { return esikle(sifirGecisGucu(filtrele(g, log7)), t); }, 100 },
        // Canny: alt esik = ust esik / 2 (yaygin kullanilan 1:2 orani)
        { "Gauss + Canny",     [](const Mat& g, double t) { Mat k; Canny(hafifYumusat(g), k, t / 2, t); return k; }, 600 },
    };

    // --- Dogruluk: sentetik goruntu, her yontem icin en iyi esik ---
    // Gercek goruntu icin esikler dusuk gurultulu (sigma = 5) durumdan alinir.
    vector<double> esikler(yontemler.size());
    for (double sigma : { 5.0, 12.0, 25.0 })
    {
        Mat temiz, gurultulu, gercek;
        sentetikGoruntu(temiz, gurultulu, gercek, sigma);
        if (sigma == 25.0)
        {
            imwrite("sentetik_gurultulu.png", gurultulu);
            imwrite("sentetik_gercek_kenar.png", gercek);
        }

        cout << "Dogruluk (sentetik goruntu, gurultu sigma = " << sigma << ", 2 piksel tolerans):" << endl;
        cout << "  Yontem                F1     Kesinlik  Duyarlilik  Kenar kalinligi*" << endl;
        for (size_t i = 0; i < yontemler.size(); i++)
        {
            double t;
            Dogruluk d = enIyiEsikAra([&](double e) { return yontemler[i].kenar(gurultulu, e); },
                                      yontemler[i].esikUst, gercek, &t);
            if (sigma == 5.0) esikler[i] = t;
            Mat k = yontemler[i].kenar(gurultulu, t);
            // kalinlik: bulunan kenar pikseli / gercek kenar uzunlugu (yaklasik)
            double kalinlik = countNonZero(k) / (countNonZero(gercek) / 2.0);
            cout << "  " << setw(20) << left << yontemler[i].ad << right << setw(7) << d.f1 << setw(10)
                << d.kesinlik << setw(12) << d.duyarlilik << setw(12) << kalinlik << " px" << endl;

            if (sigma == 25.0)
            {
                string dosya = dosyaAdi(yontemler[i].ad);
                imwrite("sentetik_" + dosya + ".png", k);
            }
        }
        cout << endl;
    }
    cout << "* gercek kenar 1 piksel kalinliginda kabul edilirse ortalama kenar kalinligi" << endl;

    // --- Calisma zamani: gercek goruntu ---
    Mat gri = griOku("resim.jpg");
    cout << "\nCalisma zamani (gercek goruntu " << gri.cols << " x " << gri.rows << ", 30 tekrar ortalamasi):" << endl;
    cout << "  Yontem                Sure (ms)   Kenar piksel orani" << endl;
    for (size_t i = 0; i < yontemler.size(); i++)
    {
        Mat k = yontemler[i].kenar(gri, esikler[i]);
        double ms = sureOlc([&]() { yontemler[i].kenar(gri, esikler[i]); });
        cout << "  " << setw(20) << left << yontemler[i].ad << right << setw(10) << ms
            << setw(16) << 100.0 * countNonZero(k) / k.total() << " %" << endl;

        string dosya = dosyaAdi(yontemler[i].ad);
        imwrite("kenar_" + dosya + ".png", k);
    }

    // Canny'nin farkli esiklerdeki sonucu
    Mat yumusak = hafifYumusat(gri);
    for (int ust : { 100, 200, 300 })
    {
        Mat k;
        Canny(yumusak, k, ust / 2, ust);
        imwrite("canny_" + to_string(ust / 2) + "_" + to_string(ust) + ".png", k);
        cout << "  Canny esik " << ust / 2 << "/" << ust << ": kenar piksel orani = "
            << 100.0 * countNonZero(k) / k.total() << " %" << endl;
    }

    Mat canny;
    Canny(yumusak, canny, 50, 100);
    imshow("Gri", gri);
    imshow("Canny (50, 100)", canny);
    waitKey(0);
    return 0;
}
