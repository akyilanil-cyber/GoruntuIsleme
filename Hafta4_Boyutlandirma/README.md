# Hafta 4 - Dizindeki Görüntüleri 1024x768'e Boyutlandırma

## Amaç
C++ ile bir dizindeki görüntü dosyalarını okuyup 1024x768 çözünürlüğünde yeniden boyutlandırmak.

## Yöntem
1. `std::filesystem::directory_iterator` ile dizindeki dosyalar gezilir. Uzantısı resim olanlar (.jpg, .jpeg, .png, .bmp, .tif, .tiff, .webp) seçilir.
2. Her dosya `imread` ile okunur. Okunamayanlar atlanır ve raporlanır.
3. `resize` ile tam olarak 1024x768'e getirilir:
   - Küçültmede `INTER_AREA`: piksel alanlarının ortalamasını alır, örtüşme (aliasing) oluşmaz.
   - Büyütmede `INTER_CUBIC`: 4x4 komşulukla kübik ara değerleme yapar, daha yumuşak sonuç verir.
4. Sonuçlar aynı dosya adıyla çıkış dizinine kaydedilir.

Kullanım: `Hafta4_Boyutlandirma.exe [giris_dizini] [cikis_dizini]`. Varsayılan dizinler: `resimler` → `cikti_1024x768`

## Sonuçlar
| Dosya | Orijinal | Yeni | İşlem |
|---|---|---|---|
| baboon.jpg | 512x512 | 1024x768 | büyütme |
| board.jpg | 640x480 | 1024x768 | büyütme |
| building.jpg | 868x600 | 1024x768 | büyütme |
| buyuk_resim.jpg | 1066x1600 | 1024x768 | küçültme |
| fruits.jpg | 512x480 | 1024x768 | büyütme |
| lena.jpg | 512x512 | 1024x768 | büyütme |
| sudoku.png | 558x563 | 1024x768 | büyütme |

- 7 görüntü boyutlandırıldı, 0 dosya atlandı.
- 1024x768 en-boy oranı 4:3'tür. Oranı farklı görüntüler (örneğin kare 512x512 ya da dikey 1066x1600) bu boyuta getirilirken yatayda/dikeyde esner. İstenen tam olarak 1024x768 olduğu için oran korunmamıştır.
- CUDA ile GPU üzerinde aynı işlem: [Hafta4_Colab_CUDA](../Hafta4_Colab_CUDA)

## Gereksinimler
Visual Studio 2026 (C++17 ve üzeri), OpenCV 4.14.0
