# Hafta 3 - Görev 3: CLAHE

## Amaç
Düşük kontrastlı görüntüyü OpenCV'nin hazır CLAHE fonksiyonu ile düzeltmek, ardından aynı algoritmayı hazır fonksiyon kullanmadan yazıp test etmek.

## Yöntem (elle CLAHE)
1. Görüntü 8x8 karoya (tile) bölünür.
2. Her karonun histogramı çıkarılır.
3. Histogram kırpma sınırında (clip limit = 2.0) kırpılır, kesilen kısım tüm değerlere eşit dağıtılır. Böylece gürültünün aşırı büyümesi engellenir.
4. Her karo için CDF'den bir dönüşüm tablosu oluşturulur.
5. Karo sınırlarında bloklanma olmaması için her piksel, en yakın 4 karonun tablosu arasında bilineer interpolasyonla dönüştürülür.

## Test Sonucu
- Görüntü: 1066 x 1600
- Hazır CLAHE ile elle yazılan CLAHE arasındaki en büyük fark: 0
- Farklı piksel sayısı: 0 / 1.705.600
- Elle yazılan CLAHE, OpenCV ile birebir aynı sonucu vermektedir.