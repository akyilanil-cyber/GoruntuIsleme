# Hafta 4 - Ödev 6: 7x7 Laplacian of Gaussian (LoG)

## Amaç
7x7 LoG çekirdeği ile görüntüdeki kenarları bulmak. Sonucu, Gauss filtresinden geçirilmiş ve geçirilmemiş diğer kenar filtreleriyle karşılaştırıp raporlamak.

**Rapor: [Rapor.md](Rapor.md)**

## Yöntem
1. LoG çekirdeği formülden üretilir (7x7, σ = 1,0). Toplamı 0 olacak şekilde ortalaması çıkarılır.
2. Çekirdek gri görüntüye uygulanır. Kenarlar sıfır geçişlerinden bulunur.
3. Karşılaştırma: Prewitt, Sobel ve Laplacian hem yumuşatmasız hem 3x3 Gauss ile yumuşatılmış olarak çalıştırılır.
4. Doğruluk, kenarları bilinen sentetik görüntüde üç gürültü seviyesinde (σ = 5, 12, 25) F1 ile ölçülür. Gerçek görüntüde kenar oranı ve süre ölçülür.

## Sonuçlar (özet, σ = 25 gürültü)
| Yöntem | Yumuşatmasız F1 | Gauss ile F1 |
|---|---|---|
| Prewitt | 0,900 | 0,982 |
| Sobel | 0,901 | 0,981 |
| Laplacian | 0,301 | 0,508 |
| LoG 7x7 | 0,653 (yumuşatma çekirdeğin içinde) | |

- Çıktılar: kenar_*.png (gerçek görüntü), sentetik_*.png (σ = 12), log_cekirdek.png

## Gereksinimler
Visual Studio 2026, OpenCV 4.14.0. Süreler Release x64 derlemesiyle ölçülmüştür.
