# Hafta 4 - Ödev 1: Yatay Prewitt Filtresi

## Amaç
[[-1 0 1], [-1 0 1], [-1 0 1]] 3x3 Prewitt filtresini gri tonlamalı bir görüntü üzerinde test etmek. Filtreden önce görüntü Gauss filtresiyle hafifçe yumuşatılır.

## Yöntem
1. Görüntü gri seviyede (tek kanal) okunur.
2. 3x3 Gauss filtresi (sigma = 0,8) ile hafifçe yumuşatılır.
3. Prewitt çekirdeği `filter2D` ile görüntü üzerinde gezdirilir. Sonuç işaretli olduğu için ondalıklı (CV_32F) tutulur.
4. Gösterim için mutlak değer alınır ve 3'e bölünür. Çekirdekteki pozitif ağırlıkların toplamı 3 olduğundan sonuç 0-255 aralığına sığar.
5. Karşılaştırma için aynı filtre yumuşatma yapılmadan da uygulanır.

Bu çekirdek soldan sağa doğru parlaklık değişimini ölçer. Bu yüzden **dikey kenarlarda** güçlü cevap verir. Pozitif değer soldan sağa aydınlanma, negatif değer kararma demektir.

## Sonuçlar
Görüntü: 868 x 600 (building.jpg)

| | Ortalama \|cevap\| | Kenar piksel (> 40) |
|---|---|---|
| Yumuşatmasız | 10,40 | 30247 |
| Gauss + Prewitt | 9,07 | 24840 |

- Cevap aralığı: -476 .. +475
- Gauss yumuşatması, dokulu bölgelerdeki (ağaçlar, çimen) küçük parlaklık dalgalanmalarını bastırdığı için eşiği geçen piksel sayısı %18 azalmıştır. Binanın dikey sınırları ise korunmuştur.
- Çıktılar: prewitt_x.png, prewitt_x_yumusatmasiz.png

## Gereksinimler
Visual Studio 2026, OpenCV 4.14.0
