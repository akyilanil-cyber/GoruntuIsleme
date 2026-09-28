// Hafta 2 - Odev 2: Renk Nicemleme (Color Quantization) - K-Means ile
// Kaynak fikir: pyimagesearch.com "Color Quantization with OpenCV using K-Means Clustering"
// Python/scikit-learn yerine C++ ve OpenCV'nin cv::kmeans fonksiyonu kullanildi.

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>

using namespace cv;
using namespace std;

// Goruntuyu k renge indirir
Mat renkNicemle(const Mat& bgr, int k)
{
    // 1) BGR -> Lab (Lab'da renk mesafesi goz algisina daha yakin)
    Mat lab;
    cvtColor(bgr, lab, COLOR_BGR2Lab);

    // 2) (yukseklik x genislik x 3) -> (piksel sayisi x 3) tablo, float
    Mat veri = lab.reshape(1, lab.rows * lab.cols);
    veri.convertTo(veri, CV_32F);

    // 3) K-means
    Mat etiketler, merkezler;
    TermCriteria durma(TermCriteria::EPS + TermCriteria::COUNT, 10, 1.0);
    kmeans(veri, k, etiketler, durma, 3, KMEANS_PP_CENTERS, merkezler);

    // 4) Her pikseli kendi kumesinin merkez rengiyle degistir
    Mat nicemli(veri.rows, 3, CV_8U);
    for (int i = 0; i < veri.rows; i++)
    {
        int kume = etiketler.at<int>(i);
        for (int c = 0; c < 3; c++)
            nicemli.at<uchar>(i, c) = saturate_cast<uchar>(merkezler.at<float>(kume, c));
    }

    // 5) Tabloyu tekrar goruntu sekline getir, Lab -> BGR
    Mat nicemliLab = nicemli.reshape(3, bgr.rows);
    Mat sonuc;
    cvtColor(nicemliLab, sonuc, COLOR_Lab2BGR);
    return sonuc;
}

int main()
{
    Mat resim = imread("resim.jpg");
    if (resim.empty())
    {
        cout << "resim.jpg bulunamadi!" << endl;
        return -1;
    }

    // Buyuk resimlerde kmeans yavas calisir, genisligi 800'e indir
    if (resim.cols > 800)
    {
        double olcek = 800.0 / resim.cols;
        resize(resim, resim, Size(), olcek, olcek, INTER_AREA);
    }

    imshow("Orijinal", resim);

    vector<int> kDegerleri = { 4, 8, 16 };
    for (int k : kDegerleri)
    {
        cout << "k = " << k << " hesaplaniyor..." << endl;
        Mat sonuc = renkNicemle(resim, k);

        Mat yanYana;
        hconcat(resim, sonuc, yanYana);

        imshow("k=" + to_string(k), yanYana);
        imwrite("nicemli_k" + to_string(k) + ".png", sonuc);
    }

    cout << "Bitti. Sonuclar kaydedildi." << endl;
    waitKey(0);
    return 0;
}