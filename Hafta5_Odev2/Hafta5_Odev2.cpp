// Hafta 5 - Odev 2: Kendi calcHist fonksiyonumuz
// calcHist fonksiyonu elle yazilir ve her kanal icin ayri ayri calistirilir.
// Sonuc OpenCV'nin calcHist fonksiyonuyla karsilastirilarak dogrulanir.

#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

// Kendi calcHist fonksiyonumuz: tek kanalli 8 bit goruntu icin 256 kutulu histogram.
// Cikti OpenCV ile ayni formattadir (256x1, CV_32F), boylece dogrudan karsilastirilabilir.
void benimCalcHist(const Mat& kanal, Mat& hist)
{
    CV_Assert(kanal.type() == CV_8UC1);

    int sayac[256] = { 0 };
    for (int y = 0; y < kanal.rows; y++)
    {
        const uchar* p = kanal.ptr<uchar>(y);
        for (int x = 0; x < kanal.cols; x++)
            sayac[p[x]]++;   // piksel degeri = kutu numarasi
    }

    hist.create(256, 1, CV_32F);
    for (int i = 0; i < 256; i++)
        hist.at<float>(i) = (float)sayac[i];
}

// Uc kanalin histogramini tek grafikte cizer (B mavi, G yesil, R kirmizi)
Mat histogramlariCiz(const Mat hist[3])
{
    const int G = 512, Y = 400;
    Mat tuval(Y, G, CV_8UC3, Scalar(255, 255, 255));
    const Scalar renkler[3] = { Scalar(255, 0, 0), Scalar(0, 160, 0), Scalar(0, 0, 255) };

    double enBuyuk = 0;
    for (int k = 0; k < 3; k++)
    {
        double m;
        minMaxLoc(hist[k], nullptr, &m);
        enBuyuk = max(enBuyuk, m);
    }
    for (int k = 0; k < 3; k++)
        for (int i = 1; i < 256; i++)
        {
            int y0 = Y - cvRound(hist[k].at<float>(i - 1) / enBuyuk * (Y - 10));
            int y1 = Y - cvRound(hist[k].at<float>(i) / enBuyuk * (Y - 10));
            line(tuval, Point((i - 1) * 2, y0), Point(i * 2, y1), renkler[k], 2, LINE_AA);
        }
    return tuval;
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

    vector<Mat> kanallar;
    split(kaynak, kanallar);
    const char* adlar[3] = { "B", "G", "R" };

    int boyut = 256;
    float aralik[] = { 0, 256 };
    const float* araliklar[] = { aralik };

    Mat benimHist[3];
    cout << "\nKanal | Toplam   | OpenCV ile mutlak fark toplami" << endl;
    for (int k = 0; k < 3; k++)
    {
        benimCalcHist(kanallar[k], benimHist[k]);

        // Dogrulama: OpenCV'nin hazir fonksiyonu
        Mat opencvHist;
        calcHist(&kanallar[k], 1, 0, Mat(), opencvHist, 1, &boyut, araliklar);
        double fark = norm(benimHist[k], opencvHist, NORM_L1);

        cout << "  " << adlar[k] << "   | " << sum(benimHist[k])[0] << "   | " << fark
            << (fark == 0 ? " (dogru)" : " (HATA)") << endl;
    }

    // Ornek kutular
    cout << "\nDeger |   B   |   G   |   R" << endl;
    for (int v : { 0, 64, 128, 192, 255 })
        cout << v << "   | " << benimHist[0].at<float>(v) << " | " << benimHist[1].at<float>(v)
            << " | " << benimHist[2].at<float>(v) << endl;

    Mat cizim = histogramlariCiz(benimHist);
    imshow("Orijinal", kaynak);
    imshow("Kendi calcHist - B, G, R", cizim);
    imwrite("hist_kendi.png", cizim);

    waitKey(0);
    return 0;
}
