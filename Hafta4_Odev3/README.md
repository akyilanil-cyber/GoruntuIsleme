# Hafta 4 - Ödev 3: Yatay ve Dikey Sobel Filtreleri

## Amaç
3x3 yatay ve dikey Sobel filtrelerini, önce Gauss filtresiyle yumuşatılmış gri görüntüye uygulamak.

## Yöntem
| Sobel X | Sobel Y |
|---|---|
| [-1 0 1]<br>[-2 0 2]<br>[-1 0 1] | [-1 -2 -1]<br>[0 0 0]<br>[1 2 1] |

1. Görüntü gri seviyede okunur ve 3x3 Gauss filtresi (sigma = 0,8) ile yumuşatılır.
2. İki çekirdek `filter2D` ile uygulanır.
3. Doğrulama için sonuç OpenCV'nin `Sobel` fonksiyonuyla karşılaştırılır.
4. `cartToPolar` ile gradyan büyüklüğü ve yönü hesaplanır. Büyüklük 4'e bölünerek 0-255 aralığına getirilir.
5. Yön bilgisi renkle gösterilir: renk tonu kenarın yönünü, parlaklık ise kenarın gücünü gösterir.

Sobel, Prewitt'ten farklı olarak orta satıra/sütuna 2 ağırlık verir. Bu, türevle birlikte dik yönde küçük bir yumuşatma da yapar.

## Sonuçlar
| | Ortalama | Kenar piksel (> 40) |
|---|---|---|
| Sobel X | 9,15 | 25335 |
| Sobel Y | 10,53 | 38423 |
| Büyüklük | 16,13 | 63020 |

- OpenCV `Sobel` ile en büyük fark: X = 0, Y = 0 (çekirdekler doğru).
- Çıktılar: sobel_x.png, sobel_y.png, sobel_buyukluk.png, sobel_yon.png

## Gereksinimler
Visual Studio 2026, OpenCV 4.14.0
