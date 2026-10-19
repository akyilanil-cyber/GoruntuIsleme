# Hafta 5 - Ödev 2: Kendi calcHist Fonksiyonumuz

## Amaç
calcHist fonksiyonunu elle yazmak ve her kanal için ayrı ayrı çalıştırmak.

## Yöntem
1. Görüntü renkli okunur ve `split` ile B, G, R kanallarına ayrılır.
2. `benimCalcHist` fonksiyonu 256 elemanlı bir sayaç dizisi tutar. Her pikselin değeri, artırılacak kutunun numarasıdır.
3. Sonuç OpenCV ile aynı formatta (256x1, CV_32F) döndürülür.
4. Doğrulama için her kanalda OpenCV'nin `calcHist` fonksiyonu da çalıştırılır ve iki histogram arasındaki mutlak farkların toplamı hesaplanır.

## Sonuçlar
| Kanal | Toplam | OpenCV ile fark |
|---|---|---|
| B | 254592 | 0 |
| G | 254592 | 0 |
| R | 254592 | 0 |

| Değer | B | G | R |
|---|---|---|---|
| 0 | 41499 | 10595 | 2433 |
| 64 | 1002 | 798 | 577 |
| 128 | 527 | 927 | 489 |
| 192 | 333 | 805 | 668 |
| 255 | 2556 | 1432 | 51777 |

- Kendi fonksiyonumuz üç kanalda da OpenCV ile birebir aynı histogramı verir.
- Üç kanalın histogramı tek grafikte hist_kendi.png olarak kaydedilmiştir.

## Gereksinimler
Visual Studio 2026, OpenCV 4.14.0
