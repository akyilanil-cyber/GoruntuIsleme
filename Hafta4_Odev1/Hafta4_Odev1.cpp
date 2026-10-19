// Hafta 4 - Odev 1: Yatay yonde Prewitt filtresi
// Cekirdek: [[-1 0 1], [-1 0 1], [-1 0 1]]
// Gri goruntu once Gauss filtresiyle hafifce yumusatilir, sonra filtre uygulanir.
// Bu cekirdek soldan saga parlaklik degisimini olcer, yani DIKEY kenarlari bulur.

#include "../Hafta4_Ortak/kenar_ortak.hpp"

int main()
{
    // OpenCV'nin INFO mesajlarini kapat
    utils::logging::setLogLevel(utils::logging::LOG_LEVEL_ERROR);

    Mat gri = griOku("resim.jpg");
    cout << "Goruntu: " << gri.cols << " x " << gri.rows << " (gri, tek kanal)" << endl;

    Mat cekirdek = prewittX();
    cout << "Cekirdek:\n" << cekirdek << endl;

    // 1) Gauss ile hafif yumusatma (3x3, sigma = 0.8)
    Mat yumusak = hafifYumusat(gri);

    // 2) Prewitt filtresi (sonuc isaretli: -765 .. +765)
    Mat cevap = filtrele(yumusak, cekirdek);
    Mat cevapYumusatmasiz = filtrele(gri, cekirdek);   // karsilastirma icin

    double enKucuk, enBuyuk;
    minMaxLoc(cevap, &enKucuk, &enBuyuk);
    cout << "\nCevap araligi: " << enKucuk << " .. " << enBuyuk << endl;
    cout << "Isaret: pozitif = soldan saga aydinlanma, negatif = kararma" << endl;

    // 3) Gosterim: mutlak deger / 3 -> 0-255
    Mat sonuc = gosterim(cevap, 1.0 / 3);
    Mat sonucYumusatmasiz = gosterim(cevapYumusatmasiz, 1.0 / 3);

    cout << "\n                 Ortalama |cevap|   Kenar piksel (>40)" << endl;
    cout << "Yumusatmasiz     " << setw(16) << mean(sonucYumusatmasiz)[0]
        << "   " << countNonZero(sonucYumusatmasiz > 40) << endl;
    cout << "Gauss + Prewitt  " << setw(16) << mean(sonuc)[0]
        << "   " << countNonZero(sonuc > 40) << endl;

    imshow("Gri", gri);
    imshow("Gauss + Prewitt X", sonuc);
    imshow("Yumusatmasiz Prewitt X", sonucYumusatmasiz);
    imwrite("prewitt_x.png", sonuc);
    imwrite("prewitt_x_yumusatmasiz.png", sonucYumusatmasiz);

    waitKey(0);
    return 0;
}
