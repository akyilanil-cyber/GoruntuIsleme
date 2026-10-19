// Hafta 5 - Odev 1: Kanal histogramlari (OpenCV calcHist)
// Renkli bir goruntu OpenCV ile okunur.
// Her kanal (B, G, R) icin ayri ayri histogram cikarilir ve cizdirilir.

#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

// Tek kanalin histogramini 256x1 CV_32F olarak hesaplar
Mat kanalHistogrami(const Mat& kanal)
{
    Mat hist;
    int boyut = 256;
    float aralik[] = { 0, 256 };
    const float* araliklar[] = { aralik };
    calcHist(&kanal, 1, 0, Mat(), hist, 1, &boyut, araliklar);
    return hist;
}

// Histogrami beyaz zemin uzerine verilen renkte cizer
Mat histogramCiz(const Mat& hist, Scalar renk)
{
    const int G = 512, Y = 300;
    Mat tuval(Y, G, CV_8UC3, Scalar(255, 255, 255));
    double enBuyuk;
    minMaxLoc(hist, nullptr, &enBuyuk);
    for (int i = 0; i < 256; i++)
    {
        int h = cvRound(hist.at<float>(i) / enBuyuk * (Y - 10));
        rectangle(tuval, Point(i * 2, Y - h), Point(i * 2 + 1, Y - 1), renk, FILLED);
    }
    return tuval;
}

int main()
{
    // OpenCV'nin INFO mesajlarini kapat
    utils::logging::setLogLevel(utils::logging::LOG_LEVEL_ERROR);

    Mat kaynak = imread("resim.jpg", IMREAD_COLOR); // OpenCV BGR sirasiyla okur
    if (kaynak.empty())
    {
        cout << "resim.jpg bulunamadi!" << endl;
        return -1;
    }
    cout << "Goruntu: " << kaynak.cols << " x " << kaynak.rows
        << " (" << kaynak.total() << " piksel, " << kaynak.channels() << " kanal)" << endl;

    // Kanallari ayir: kanallar[0] = B, [1] = G, [2] = R
    vector<Mat> kanallar;
    split(kaynak, kanallar);

    const char* adlar[3] = { "B (mavi)", "G (yesil)", "R (kirmizi)" };
    const char* dosyalar[3] = { "hist_B.png", "hist_G.png", "hist_R.png" };
    const Scalar renkler[3] = { Scalar(255, 0, 0), Scalar(0, 160, 0), Scalar(0, 0, 255) };

    for (int k = 0; k < 3; k++)
    {
        Mat hist = kanalHistogrami(kanallar[k]);

        double enBuyuk;
        Point enBuyukYer;
        minMaxLoc(hist, nullptr, &enBuyuk, nullptr, &enBuyukYer);
        double ortalama = mean(kanallar[k])[0];

        cout << adlar[k] << ": toplam = " << sum(hist)[0]
            << ", en sik deger = " << enBuyukYer.y << " (" << enBuyuk << " kez)"
            << ", ortalama = " << ortalama << endl;

        Mat cizim = histogramCiz(hist, renkler[k]);
        imshow(adlar[k], cizim);
        imwrite(dosyalar[k], cizim);
    }

    imshow("Orijinal", kaynak);
    waitKey(0);
    return 0;
}
