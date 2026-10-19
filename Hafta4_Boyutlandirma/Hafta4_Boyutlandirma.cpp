// Hafta 4 - Bir dizindeki goruntu dosyalarini okuyup 1024x768 cozunurluge boyutlandirma
// Kullanim: Hafta4_Boyutlandirma.exe [giris_dizini] [cikis_dizini]
// Varsayilan: giris = "resimler", cikis = "cikti_1024x768"
// Kucultmede INTER_AREA (ortusme yok), buyutmede INTER_CUBIC kullanilir.

#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <iostream>
#include <iomanip>
#include <filesystem>
#include <string>
#include <algorithm>

using namespace cv;
using namespace std;
namespace fs = std::filesystem;

const int HEDEF_G = 1024, HEDEF_Y = 768;

bool resimMi(const fs::path& yol)
{
    string uz = yol.extension().string();
    transform(uz.begin(), uz.end(), uz.begin(), ::tolower);
    for (const char* u : { ".jpg", ".jpeg", ".png", ".bmp", ".tif", ".tiff", ".webp" })
        if (uz == u) return true;
    return false;
}

int main(int argc, char** argv)
{
    // OpenCV'nin INFO mesajlarini kapat
    utils::logging::setLogLevel(utils::logging::LOG_LEVEL_ERROR);

    fs::path giris = argc > 1 ? argv[1] : "resimler";
    fs::path cikis = argc > 2 ? argv[2] : "cikti_1024x768";

    if (!fs::is_directory(giris))
    {
        cout << giris.string() << " dizini bulunamadi!" << endl;
        return -1;
    }
    fs::create_directories(cikis);

    int basarili = 0, atlanan = 0;
    cout << left << setw(28) << "Dosya" << setw(14) << "Orijinal" << "Yeni" << endl;
    for (const auto& oge : fs::directory_iterator(giris))
    {
        if (!oge.is_regular_file() || !resimMi(oge.path())) continue;

        Mat resim = imread(oge.path().string(), IMREAD_UNCHANGED);
        if (resim.empty())
        {
            cout << oge.path().filename().string() << " okunamadi, atlandi" << endl;
            atlanan++;
            continue;
        }

        // Kucultuluyorsa INTER_AREA, buyutuluyorsa INTER_CUBIC
        bool kucultme = resim.cols * resim.rows > HEDEF_G * HEDEF_Y;
        Mat yeni;
        resize(resim, yeni, Size(HEDEF_G, HEDEF_Y), 0, 0, kucultme ? INTER_AREA : INTER_CUBIC);

        fs::path hedef = cikis / oge.path().filename();
        imwrite(hedef.string(), yeni);

        string orj = to_string(resim.cols) + "x" + to_string(resim.rows);
        string yen = to_string(yeni.cols) + "x" + to_string(yeni.rows);
        cout << left << setw(28) << oge.path().filename().string() << setw(14) << orj << yen
            << (kucultme ? "  (kucultme)" : "  (buyutme)") << endl;
        basarili++;
    }

    cout << "\n" << basarili << " goruntu boyutlandirildi, " << atlanan << " dosya atlandi." << endl;
    cout << "Cikti dizini: " << fs::absolute(cikis).string() << endl;
    return 0;
}
