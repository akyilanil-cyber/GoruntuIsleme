# Hafta 4 - Ödev 5: Laplacian ile Kenar Tespiti

## Amaç
3x3 Laplacian çekirdeğiyle kenarları bulmak (öncesinde Gauss yumuşatması) ve sonuçları Sobel ve Prewitt ile karşılaştırmak.

**Karşılaştırma dokümanı: [Karsilastirma.md](Karsilastirma.md)**

## Yöntem
1. Görüntü gri okunur ve 3x3 Gauss filtresi (sigma = 0,8) ile yumuşatılır.
2. Laplacian çekirdeği [[0 1 0], [1 -4 1], [0 1 0]] uygulanır (ikinci türev).
3. Kenarlar sıfır geçişlerinden bulunur. Bir pikselin karşılıklı iki komşusu zıt işaretliyse aradaki fark kenar gücü olarak alınır ve eşiklenir.
4. Sobel ve Prewitt ile aynı koşullarda karşılaştırılır: kenarları bilinen sentetik görüntüde F1 ve yanlış kenar sayısı, gerçek görüntüde kenar oranı ve süre.

## Sonuçlar (özet)
| Gürültü | Prewitt F1 | Sobel F1 | Laplacian F1 |
|---|---|---|---|
| σ = 5 | 1,000 | 1,000 | 1,000 |
| σ = 12 | 1,000 | 1,000 | 0,944 |
| σ = 25 | 0,982 | 0,981 | 0,508 |

- Laplacian ince ve kapalı kenarlar verir, ancak gürültüye çok daha hassastır.
- Çıktılar: kenar_*.png (gerçek görüntü), sentetik_*.png (σ = 12), laplacian_ham.png (işaretli cevap, gri 128 = sıfır)

## Gereksinimler
Visual Studio 2026, OpenCV 4.14.0. Süreler Release x64 derlemesiyle ölçülmüştür.
