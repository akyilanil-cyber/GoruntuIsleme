// Hafta 4 - Odev 3: 3x3 yatay ve dikey Sobel filtreleri
// Sobel X: [[-1 0 1], [-2 0 2], [-1 0 1]]
// Sobel Y: [[-1 -2 -1], [0 0 0], [1 2 1]]
// Gri goruntu once Gauss filtresiyle hafifce yumusatilir, sonra filtreler uygulanir.
// Sonuc OpenCV'nin Sobel fonksiyonuyla dogrulanir.

#include "../Hafta4_Ortak/kenar_ortak.hpp"

int main()
{
    // OpenCV'nin INFO mesajlarini kapat
    utils::logging::setLogLevel(utils::logging::LOG_LEVEL_ERROR);

    Mat gri = griOku("resim.jpg");
    cout << "Goruntu: " << gri.cols << " x " << gri.rows << " (gri, tek kanal)" << endl;
    cout << "Sobel X:\n" << sobelX() << "\nSobel Y:\n" << sobelY() << endl;

    // 1) Gauss ile hafif yumusatma (3x3, sigma = 0.8)
    Mat yumusak = hafifYumusat(gri);

    // 2) Cekirdekleri uygula (sonuclar isaretli: -1020 .. +1020)
    Mat gx = filtrele(yumusak, sobelX());
    Mat gy = filtrele(yumusak, sobelY());

    // 3) Dogrulama: OpenCV'nin hazir Sobel fonksiyonu
    Mat cvX, cvY;
    Sobel(yumusak, cvX, CV_32F, 1, 0, 3, 1, 0, BORDER_REFLECT);
    Sobel(yumusak, cvY, CV_32F, 0, 1, 3, 1, 0, BORDER_REFLECT);
    cout << "\nOpenCV Sobel ile en buyuk fark: X = " << norm(gx, cvX, NORM_INF)
        << ", Y = " << norm(gy, cvY, NORM_INF) << endl;

    // 4) Gradyan buyuklugu ve yonu
    Mat buyukluk, aci;
    cartToPolar(gx, gy, buyukluk, aci, true);
    buyukluk /= 4;   // 0-255 araligina olcekle (Sobel agirliklari toplami 4)

    Mat sx = gosterim(gx, 1.0 / 4), sy = gosterim(gy, 1.0 / 4), sb;
    buyukluk.convertTo(sb, CV_8U);

    cout << "\n             Ortalama   Kenar piksel (>40)" << endl;
    cout << "Sobel X      " << setw(8) << mean(sx)[0] << "   " << countNonZero(sx > 40) << endl;
    cout << "Sobel Y      " << setw(8) << mean(sy)[0] << "   " << countNonZero(sy > 40) << endl;
    cout << "Buyukluk     " << setw(8) << mean(sb)[0] << "   " << countNonZero(sb > 40) << endl;

    // Yon bilgisini renkle gostermek icin: ton = yon, parlaklik = buyukluk
    Mat hsv[3] = { Mat(aci / 2), Mat(gri.size(), CV_32F, Scalar(255)), buyukluk }, yonRenkli;
    for (Mat& m : hsv) m.convertTo(m, CV_8U);
    merge(hsv, 3, yonRenkli);
    cvtColor(yonRenkli, yonRenkli, COLOR_HSV2BGR);

    imshow("Gri", gri);
    imshow("Sobel X (dikey kenarlar)", sx);
    imshow("Sobel Y (yatay kenarlar)", sy);
    imshow("Sobel buyukluk", sb);
    imshow("Gradyan yonu (renk) ve buyuklugu", yonRenkli);
    imwrite("sobel_x.png", sx);
    imwrite("sobel_y.png", sy);
    imwrite("sobel_buyukluk.png", sb);
    imwrite("sobel_yon.png", yonRenkli);

    waitKey(0);
    return 0;
}
