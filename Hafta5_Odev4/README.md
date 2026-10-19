# Hafta 5 - Ödev 4: Renk Uzayı Dönüşümleri

## Amaç
Bir renk uzayındaki görüntüyü diğer renk uzaylarına dönüştüren fonksiyonlar yazmak, bunları fonksiyon pointer'ları ile çağırmak ve sonuçları OpenCV'nin renk uzayı fonksiyonlarıyla sayısal olarak karşılaştırmak.

## Yöntem
1. Tüm dönüşüm fonksiyonları aynı imzaya sahiptir: `typedef void (*DonusumFonk)(const Mat&, Mat&);`
2. Fonksiyonlar bir tabloda tutulur (ad, fonksiyon pointer'ı, OpenCV kodu). Döngü her satırda fonksiyonu pointer üzerinden çağırır.
3. Her çıktı `cvtColor` ile aynı dönüşümün sonucuyla karşılaştırılır ve mutlak farkların toplamı yazdırılır.
4. OpenCV'nin 8 bitlik görüntülerde kullandığı tam sayı (sabit noktalı) katsayılar kullanıldığı için sonuçlar birebir aynıdır.

| Dönüşüm | Formül |
|---|---|
| BGR → GRAY | Y = 0,299 R + 0,587 G + 0,114 B |
| BGR → RGB | Kanal sırası ters çevrilir |
| BGR → HSV | V = max, S = (V − min) / V × 255, H = 0–180 (derece / 2) |
| BGR → YCrCb | Cr = (R − Y) × 0,713 + 128, Cb = (B − Y) × 0,564 + 128 |
| BGR → XYZ | sRGB → XYZ matrisi (D65 beyaz noktası) |

## Sonuçlar
| Dönüşüm | Mutlak fark toplamı | Farklı değer |
|---|---|---|
| BGR → GRAY | 0 | 0 |
| BGR → RGB | 0 | 0 |
| BGR → HSV | 0 | 0 |
| BGR → YCrCb | 0 | 0 |
| BGR → XYZ | 0 | 0 |

- Beklenen hata değeri olan 0 tüm dönüşümlerde elde edilmiştir.
- HSV'de ton (H) 8 bite sığması için 0–180 aralığında tutulur (gerçek derece = H × 2).
- Çıktılar cikti_*.png olarak kaydedilmiştir.

## Gereksinimler
Visual Studio 2026, OpenCV 4.14.0
