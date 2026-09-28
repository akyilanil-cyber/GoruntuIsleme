// Hafta 2 - Odev 1: Farkli cozunurluklerde yeniden boyutlandirma
// Ekran cozunurlugu taban alinir, goruntu x2, x4, x0.5, x0.25 olcekleriyle
// hem en yakin komsu (INTER_NEAREST) hem de kaliteli yontemle boyutlandirilir.

#define NOMINMAX
#include <windows.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace cv;
using namespace std;

// Kameradan SPACE ile tek kare ceker
Mat kameradanCek()
{
    VideoCapture cap(0);
    if (!cap.isOpened()) return Mat();

    Mat kare, foto;
    cout << "Fotograf icin SPACE, cikis icin ESC." << endl;
    while (true)
    {
        cap >> kare;
        if (kare.empty()) break;
        imshow("Kamera", kare);
        int tus = waitKey(30);
        if (tus == 32) { foto = kare.clone(); break; }
        if (tus == 27) break;
    }
    destroyWindow("Kamera");
    return foto;
}

int main()
{
    // 1) Ekran cozunurlugunu al (Windows olcekleme %125 vs. ise dogru deger icin)
    SetProcessDPIAware();
    int ekranW = GetSystemMetrics(SM_CXSCREEN);
    int ekranH = GetSystemMetrics(SM_CYSCREEN);
    cout << "Ekran cozunurlugu: " << ekranW << " x " << ekranH << endl;

    // 2) Kaynak goruntu: once dosya, yoksa kamera
    Mat kaynak = imread("dama.jpg");
    if (kaynak.empty())
    {
        cout << "dama.jpg bulunamadi, kamera aciliyor..." << endl;
        kaynak = kameradanCek();
    }
    if (kaynak.empty())
    {
        cout << "Goruntu alinamadi!" << endl;
        return -1;
    }

    // 3) Tabani ekran cozunurlugune getir
    Mat taban;
    resize(kaynak, taban, Size(ekranW, ekranH), 0, 0, INTER_AREA);
    imwrite("taban.png", taban);

    // 4) Olcekler
    vector<double> olcekler = { 2.0, 4.0, 0.5, 0.25 };
    vector<string> adlar = { "x2",  "x4", "x0.5", "x0.25" };

    for (size_t i = 0; i < olcekler.size(); i++)
    {
        double s = olcekler[i];

        // En yakin komsu: hizli ama kucultmede moire/kirilma, buyutmede pikselli
        Mat enYakin;
        resize(taban, enYakin, Size(), s, s, INTER_NEAREST);

        // Kaliteli: kucultmede INTER_AREA, buyutmede INTER_CUBIC
        Mat kaliteli;
        int yontem = (s < 1.0) ? INTER_AREA : INTER_CUBIC;
        resize(taban, kaliteli, Size(), s, s, yontem);

        cout << adlar[i] << " -> " << kaliteli.cols << " x " << kaliteli.rows << endl;

        imwrite(adlar[i] + "_nearest.png", enYakin);
        imwrite(adlar[i] + "_kaliteli.png", kaliteli);

        // Buyuk goruntuler ekrana sigmaz, pencere boyutu ayarlanabilir olsun
        int pencereTipi = (s > 1.0) ? WINDOW_NORMAL : WINDOW_AUTOSIZE;
        namedWindow(adlar[i] + " nearest", pencereTipi);
        namedWindow(adlar[i] + " kaliteli", pencereTipi);
        imshow(adlar[i] + " nearest", enYakin);
        imshow(adlar[i] + " kaliteli", kaliteli);
    }

    cout << "Tum sonuclar .png olarak proje klasorune kaydedildi." << endl;
    waitKey(0);
    return 0;
}