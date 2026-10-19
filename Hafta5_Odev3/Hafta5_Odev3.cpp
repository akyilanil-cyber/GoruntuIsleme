// Hafta 5 - Odev 3: RGB -> Gri donusumu
// Renkli goruntu tek kanalli gri tonlu goruntuye donusturulur.
// Hem OpenCV'nin hazir fonksiyonu (cvtColor) hem kendi fonksiyonumuz kullanilir.
// Eslesen piksellerin mutlak farklarinin toplami hata degeri olarak raporlanir.

#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// Gri = 0.299 R + 0.587 G + 0.114 B
// OpenCV 8 bit goruntulerde bu katsayilari 2^15 = 32768 ile carpilmis
// tamsayilar olarak kullanir (toplamlari tam 32768). Ayni katsayilari
// kullandigimiz icin sonuc OpenCV ile birebir ayni olur.
const int KAYDIRMA = 15;
const int R_KAT = 9798;   // round(0.299 * 32768)
const int G_KAT = 19235;  // round(0.587 * 32768)
const int B_KAT = 3735;   // round(0.114 * 32768)

void benimGriyeCevir(const Mat& giris, Mat& cikis)
{
    CV_Assert(giris.type() == CV_8UC3);
    cikis.create(giris.size(), CV_8UC1);   // tek kanal

    for (int y = 0; y < giris.rows; y++)
    {
        const Vec3b* g = giris.ptr<Vec3b>(y);
        uchar* c = cikis.ptr<uchar>(y);
        for (int x = 0; x < giris.cols; x++)
        {
            int B = g[x][0], G = g[x][1], R = g[x][2];   // OpenCV sirasi: B, G, R
            int toplam = B * B_KAT + G * G_KAT + R * R_KAT;
            // + 2^14 yuvarlama icin, >> 15 ise 32768'e bolme
            c[x] = saturate_cast<uchar>((toplam + (1 << (KAYDIRMA - 1))) >> KAYDIRMA);
        }
    }
}

// Karsilastirma icin: ayni formulun ondalikli (double) "ders kitabi" hali
void ondalikliGriyeCevir(const Mat& giris, Mat& cikis)
{
    cikis.create(giris.size(), CV_8UC1);
    for (int y = 0; y < giris.rows; y++)
    {
        const Vec3b* g = giris.ptr<Vec3b>(y);
        uchar* c = cikis.ptr<uchar>(y);
        for (int x = 0; x < giris.cols; x++)
            c[x] = saturate_cast<uchar>(0.114 * g[x][0] + 0.587 * g[x][1] + 0.299 * g[x][2]);
    }
}

int main()
{
    // OpenCV'nin INFO mesajlarini kapat
    utils::logging::setLogLevel(utils::logging::LOG_LEVEL_ERROR);

    Mat kaynak = imread("resim.jpg", IMREAD_COLOR);
    if (kaynak.empty())
    {
        cout << "resim.jpg bulunamadi!" << endl;
        return -1;
    }
    cout << "Goruntu: " << kaynak.cols << " x " << kaynak.rows
        << " (" << kaynak.total() << " piksel)" << endl;

    // 1) OpenCV'nin hazir fonksiyonu
    Mat opencvGri;
    cvtColor(kaynak, opencvGri, COLOR_BGR2GRAY);

    // 2) Kendi fonksiyonumuz
    Mat benimGri;
    benimGriyeCevir(kaynak, benimGri);

    // 3) Ondalikli formul (karsilastirma icin)
    Mat ondalikliGri;
    ondalikliGriyeCevir(kaynak, ondalikliGri);

    cout << "Cikti: " << benimGri.channels() << " kanal" << endl;

    // Hata degeri = eslesen piksellerin mutlak farklarinin toplami
    Mat fark;
    absdiff(opencvGri, benimGri, fark);
    double hata = sum(fark)[0];
    cout << "\nKendi fonksiyon vs OpenCV" << endl;
    cout << "  Mutlak fark toplami (hata): " << hata << (hata == 0 ? " (dogru)" : "") << endl;
    cout << "  Farkli piksel sayisi      : " << countNonZero(fark) << endl;

    absdiff(opencvGri, ondalikliGri, fark);
    cout << "\nOndalikli formul vs OpenCV" << endl;
    cout << "  Mutlak fark toplami (hata): " << sum(fark)[0] << endl;
    cout << "  Farkli piksel sayisi      : " << countNonZero(fark) << endl;

    // Ornek pikseller
    cout << "\nOrnek pikseller (B, G, R) -> OpenCV / Kendi" << endl;
    for (Point p : { Point(0, 0), Point(kaynak.cols / 2, kaynak.rows / 2), Point(kaynak.cols - 1, kaynak.rows - 1) })
    {
        Vec3b v = kaynak.at<Vec3b>(p);
        cout << "  (" << (int)v[0] << ", " << (int)v[1] << ", " << (int)v[2] << ") -> "
            << (int)opencvGri.at<uchar>(p) << " / " << (int)benimGri.at<uchar>(p) << endl;
    }

    imshow("Orijinal", kaynak);
    imshow("OpenCV cvtColor", opencvGri);
    imshow("Kendi fonksiyon", benimGri);
    imwrite("gri_opencv.png", opencvGri);
    imwrite("gri_kendi.png", benimGri);

    waitKey(0);
    return 0;
}
