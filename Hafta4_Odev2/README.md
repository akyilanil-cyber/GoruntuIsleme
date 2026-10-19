# Hafta 4 - Ödev 2: Dikey Prewitt Filtresi

## Amaç
[[-1 -1 -1], [0 0 0], [1 1 1]] 3x3 Prewitt filtresini gri tonlamalı bir görüntü üzerinde test etmek. Filtreden önce görüntü Gauss filtresiyle hafifçe yumuşatılır.

## Yöntem
1. Görüntü gri seviyede okunur ve 3x3 Gauss filtresi (sigma = 0,8) ile yumuşatılır.
2. Prewitt çekirdeği `filter2D` ile uygulanır (sonuç CV_32F, işaretli).
3. Gösterim için mutlak değer / 3 alınır.
4. Ödev 1'deki yatay çekirdekle birlikte gradyan büyüklüğü de hesaplanır: √(Gx² + Gy²).

Bu çekirdek yukarıdan aşağı parlaklık değişimini ölçer. Bu yüzden **yatay kenarlarda** (pencere ve kat çizgileri) güçlü cevap verir.

## Sonuçlar
Görüntü: 868 x 600 (building.jpg)

| | Ortalama \|cevap\| | Kenar piksel (> 40) |
|---|---|---|
| Yumuşatmasız | 11,77 | 43022 |
| Gauss + Prewitt | 10,46 | 38247 |

- Cevap aralığı: -452 .. +445
- Binadaki kat ve pencere çizgileri yatay olduğu için bu yönde Ödev 1'e göre daha fazla kenar pikseli bulunmuştur (38247'ye karşı 24840).
- İki yönün birleşimi (prewitt_buyukluk.png) hem yatay hem dikey kenarları birlikte gösterir.
- Çıktılar: prewitt_y.png, prewitt_y_yumusatmasiz.png, prewitt_buyukluk.png

## Gereksinimler
Visual Studio 2026, OpenCV 4.14.0
