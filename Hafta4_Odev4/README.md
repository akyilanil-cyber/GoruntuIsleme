# Hafta 4 - Ödev 4: Sobel ve Prewitt Karşılaştırması

## Amaç
Sobel ve Prewitt operatörlerinin sonuçlarını karşılaştıran bir sayfalık metin hazırlamak.

**Karşılaştırma metni: [Karsilastirma.md](Karsilastirma.md)**

## Yöntem
Metindeki sayılar `Hafta4_Odev4.cpp` ile ölçülmüştür:
1. **Gerçek görüntü:** İki operatörün gradyan büyüklüğü haritaları ve aralarındaki fark
2. **Gürültü hassasiyeti:** Kenarların yeri bilinen sentetik görüntüye σ = 12 ve σ = 25 Gauss gürültüsü eklenir. Kenar olmayan düz bölgelerdeki cevap ve kenar/gürültü oranı ölçülür.
3. **Doğruluk:** Bilinen kenarlara göre F1, kesinlik ve duyarlılık hesaplanır (2 piksel tolerans, her yöntem için en iyi eşik).
4. **Yön doğruluğu:** Bir dairenin kenarında ölçülen gradyan yönü, gerçek yönle (merkeze doğru) karşılaştırılır.
5. **Süre:** 30 tekrarın ortalaması (Release derlemesi)

## Çıktılar
- Prewitt_buyukluk.png, Sobel_buyukluk.png: gerçek görüntüde gradyan büyüklükleri
- fark.png: iki sonuç arasındaki fark (normalize edilmiş)
- sentetik_gurultulu_12.png, sentetik_gurultulu_25.png: test görüntüleri

## Gereksinimler
Visual Studio 2026, OpenCV 4.14.0. Süreler Release x64 derlemesiyle ölçülmüştür.
