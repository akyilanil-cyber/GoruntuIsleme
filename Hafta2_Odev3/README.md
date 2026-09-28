# Hafta 2 - Ödev 3: Parçalı Histogram

## Amaç
Görüntüyü 4 parçaya bölüp her parçanın histogramını ayrı ayrı hesaplamak, sonra bu histogramları toplayarak ana görüntünün histogramını elde etmek.

## Yöntem
1. Görüntü gri seviyeye çevrilir. Her pikselin değeri 0-255 arası parlaklık değeridir.
2. Görüntü 4 çeyreğe bölünür (sol üst, sağ üst, sol alt, sağ alt).
3. Her çeyrek için ayrı bir thread açılır ve ayrı bir CPU çekirdeğine sabitlenir (SetThreadAffinityMask).
4. Her thread kendi 256 elemanlı histogram dizisine hangi parlaklık değerinden kaç piksel olduğunu sayar. Ayrı diziler kullanıldığı için thread'ler arasında çakışma (race condition) olmaz.
5. 4 histogram eleman eleman toplanarak ana görüntünün histogramı bulunur.
6. Doğrulama için tüm görüntünün histogramı tek seferde de hesaplanır ve iki sonuç karşılaştırılır.

## Sonuçlar
Görüntü boyutu: 612 x 416 (254592 piksel)

| Değer | Sol üst | Sağ üst | Sol alt | Sağ alt | TOPLAM |
|---|---|---|---|---|---|
| 0 | 6 | 18 | 8 | 12 | 44 |
| 64 | 224 | 267 | 345 | 283 | 1119 |
| 128 | 158 | 250 | 239 | 275 | 922 |
| 192 | 495 | 238 | 185 | 292 | 1210 |
| 255 | 1 | 2 | 15 | 10 | 28 |

- Parçalardan toplanan histogram, tüm görüntünün tek seferde hesaplanan histogramıyla birebir aynıdır.
- Histogramdaki toplam piksel sayısı (254592), görüntünün piksel sayısına eşittir.
- Her piksel yalnızca bir parçaya ait olduğu için parçaların toplamı ana histogramı verir.

## Gereksinimler
Visual Studio 2026, OpenCV 4.14.0