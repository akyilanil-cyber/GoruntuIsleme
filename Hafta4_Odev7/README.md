# Hafta 4 - Ödev 7: OpenCV Canny ile Kenar Tespiti

## Amaç
OpenCV'nin Canny algoritmasıyla kenarları bulmak ve sonuçları diğer algoritmalarla çalışma zamanı ve doğruluk açısından karşılaştırmak.

## Yöntem
1. Görüntü 3x3 Gauss filtresiyle (σ = 0,8) yumuşatılır ve `Canny` uygulanır. Alt eşik, üst eşiğin yarısıdır (1:2).
2. Karşılaştırılan yöntemler: Gauss + Prewitt, Gauss + Sobel, Gauss + Laplacian, 7x7 LoG ve Gauss + Canny.
3. **Doğruluk:** Kenarların yeri bilinen 400x400 sentetik görüntüde, üç gürültü seviyesinde (σ = 5, 12, 25) ölçülür. Her yöntem için F1'i en yüksek yapan eşik aranır. 2 piksel tolerans kullanılır. Ayrıca bulunan kenarların ortalama kalınlığı hesaplanır.
4. **Çalışma zamanı:** 868x600 gerçek görüntüde, yumuşatma dahil 30 tekrarın ortalaması alınır (Release x64).

## Sonuçlar
### Doğruluk (F1)
| Yöntem | σ = 5 | σ = 12 | σ = 25 | Kenar kalınlığı (σ = 25) |
|---|---|---|---|---|
| Gauss + Prewitt | 1,000 | 1,000 | 0,980 | 1,45 px |
| Gauss + Sobel | 1,000 | 1,000 | 0,981 | 1,35 px |
| Gauss + Laplacian | 1,000 | 0,944 | 0,508 | 0,63 px |
| LoG 7x7 | 1,000 | 0,969 | 0,653 | 0,69 px |
| **Gauss + Canny** | **1,000** | **1,000** | **0,998** | **0,97 px** |

### Çalışma zamanı (868x600)
| Yöntem | Süre | Kenar piksel oranı |
|---|---|---|
| Gauss + Prewitt | 6,50 ms | %21,1 |
| Gauss + Sobel | 6,64 ms | %21,2 |
| Gauss + Laplacian | 9,05 ms | %10,6 |
| LoG 7x7 | 16,66 ms | %13,6 |
| **Gauss + Canny** | **1,79 ms** | **%8,1** |

Canny'nin eşiğe göre davranışı: 50/100 → %10,8, 100/200 → %6,6, 150/300 → %4,6 kenar pikseli.

## Değerlendirme
- **Doğruluk:** Canny en güçlü gürültüde bile en doğru yöntemdir (F1 = 0,998). Kenarları tek piksel kalınlığında verir (0,97 px). Gradyan yöntemleri de doğrudur, ancak eşikleme sonrasında 1,4-2 piksel kalınlığında şeritler bırakır. Laplacian tabanlı yöntemler gürültüde yanlış kenarlar ürettiği için geride kalır.
- **Neden daha iyi:** Canny, Sobel gradyanının üzerine iki adım ekler. Non-maximum suppression kenarı tek piksele inceltir. Histerezis eşikleme de zayıf ama güçlü kenarlara bağlı pikselleri tutar, bağlantısız gürültü piksellerini atar.
- **Çalışma zamanı:** Canny daha fazla adımdan oluşmasına rağmen en hızlı yöntem olmuştur (1,8 ms). OpenCV'nin Canny'si 8 bit tam sayılarla ve SIMD ile çok iyi optimize edilmiştir. Diğer yöntemler ise ondalıklı (float) `filter2D` + `magnitude` ile hesaplanmıştır. Yani bu fark, algoritmaların kendi karmaşıklığından çok uygulamaların optimizasyon farkını gösterir. Kendi yazdığımız Canny'nin (Hafta4_Canny) süresi 58 ms'dir.
- **Sonuç:** Hem doğruluk hem kenar kalitesi açısından en iyi sonuç Canny'dendir. Dezavantajı, iki eşiğin görüntüye göre ayarlanması gerekmesidir.

## Çıktılar
- kenar_*.png: gerçek görüntüde her yöntemin sonucu
- canny_50_100.png, canny_100_200.png, canny_150_300.png: farklı eşiklerde Canny
- sentetik_gurultulu.png (σ = 25), sentetik_gercek_kenar.png, sentetik_*.png: sentetik test

## Gereksinimler
Visual Studio 2026, OpenCV 4.14.0. Süreler Release x64 derlemesiyle ölçülmüştür.
