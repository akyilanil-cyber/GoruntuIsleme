# Hafta 5 - Ödev 1: Kanal Histogramları

## Amaç
Renkli bir görüntüyü OpenCV ile okumak ve her renk kanalı için ayrı ayrı histogram çıkarmak.

## Yöntem
1. Görüntü `imread` ile renkli olarak okunur. OpenCV kanalları B, G, R sırasıyla tutar.
2. `split` ile görüntü üç tek kanallı görüntüye ayrılır.
3. Her kanal için `calcHist` ile 256 kutulu histogram hesaplanır.
4. Histogramlar çizilip hist_B.png, hist_G.png ve hist_R.png olarak kaydedilir.

## Sonuçlar
Görüntü boyutu: 612 x 416 (254592 piksel)

| Kanal | En sık değer | Kaç kez | Ortalama |
|---|---|---|---|
| B (mavi) | 0 | 41499 | 64,3 |
| G (yeşil) | 0 | 10595 | 125,1 |
| R (kırmızı) | 255 | 51777 | 181,9 |

- Her kanalın histogram toplamı piksel sayısına (254592) eşittir.
- Görüntüde kırmızı ve sarı meyveler baskın olduğu için R kanalı 255'te yığılır, B kanalı ise 0'a yakın değerlerde toplanır.

## Gereksinimler
Visual Studio 2026, OpenCV 4.14.0
