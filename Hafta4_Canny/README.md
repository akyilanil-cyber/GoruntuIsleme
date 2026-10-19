# Hafta 4 - Haftalık Ödev: Canny Kenar Bulma Algoritmasının Sıfırdan Yazılması

## Amaç
Canny kenar bulma algoritmasını hazır fonksiyon kullanmadan yazmak. Her adım ayrı bir alt fonksiyon olmalıdır.

## Yöntem
| Adım | Fonksiyon | Ne yapar |
|---|---|---|
| 1. Gürültü bastırma | `gurultuBastir` | 5x5 Gauss çekirdeği (σ = 1,4) formülden üretilir, toplamı 1'e normalize edilir ve elle konvolüsyonla uygulanır. |
| 2. Gradyan hesaplama | `sobelGradyan` | Sobel X ve Y çekirdekleri elle konvolüsyonla uygulanır. Büyüklük √(Gx² + Gy²), yön atan2(Gy, Gx) olarak hesaplanır (0-180°). |
| 3. Non-maximum suppression | `nonMaxBastirma` | Yön 4 sınıfa yuvarlanır (0°, 45°, 90°, 135°). Her piksel, gradyan yönündeki iki komşusundan büyük değilse silinir. |
| 4. Histerezis eşikleme | `histerezisEsikleme` | Büyüklük ≥ 100 ise güçlü kenar, 50-100 arası ise zayıf kenar sayılır. Güçlü kenarlardan başlayarak 8 komşulukla bağlı zayıf kenarlar takip edilir (yığın ile). |

- `konvolusyon`: kenarlarda görüntüyü aynalayan elle konvolüsyon. Hiçbir adımda `GaussianBlur`, `Sobel`, `filter2D` veya `Canny` kullanılmamıştır.
- `benimCanny`: dört adımı sırayla çağırır.
- Doğrulama: Sonuç, aynı Gauss ve aynı eşiklerle OpenCV'nin `Canny` fonksiyonuyla (L2 gradyan) karşılaştırılır.

## Sonuçlar
Görüntü: 868 x 600 (520800 piksel), eşikler 50 / 100

| Adım sonrası | Aday kenar pikseli |
|---|---|
| Gradyan > 0 | 518321 |
| Non-maximum suppression | 118616 |
| Güçlü (≥ 100) | 28435 |
| Zayıf (50-100) | 14969 |
| Histerezis sonrası (sonuç) | 35542 |

**OpenCV Canny ile karşılaştırma**
- Kenar pikseli: kendi = 35542, OpenCV = 35463
- 1 piksel toleransla uyum: F1 = 0,992
- Birebir aynı piksel oranı (arka plan dahil): %99,57
- Süre: kendi = 58 ms, OpenCV = 7 ms. OpenCV tam sayı aritmetiği ve SIMD kullanır, bizim kodumuz ise açık döngülerle yazılmıştır.
- Küçük farklar, OpenCV'nin yönü 4 sınıfa yuvarlarken kullandığı tam sayı yaklaşımından ve eşitlik durumlarındaki farklı karşılaştırma kurallarından kaynaklanır (fark_renkli.png: beyaz = ikisinde de, kırmızı = sadece bizde, yeşil = sadece OpenCV'de).

## Çıktılar
1_gauss.png, 2_sobel_gradyan.png, 3_non_max_suppression.png, 4_histerezis_sonuc.png, opencv_canny.png, fark_renkli.png

## Gereksinimler
Visual Studio 2026, OpenCV 4.14.0
