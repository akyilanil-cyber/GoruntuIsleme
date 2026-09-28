# Hafta 2 - Ödev 1: Farklı Çözünürlüklerde Yeniden Boyutlandırma

## Amaç
Kameradan çekilen bir görüntüyü ekran çözünürlüğü taban alınarak x2, x4, x0.5 ve x0.25 ölçeklerine getirmek ve farklı enterpolasyon yöntemlerini karşılaştırmak.

## Yöntem
1. Ekran çözünürlüğü GetSystemMetrics ile okunur.
2. Kameradan SPACE tuşuyla fotoğraf çekilir (proje klasöründe dama.jpg varsa o kullanılır).
3. Görüntü önce ekran çözünürlüğüne getirilir.
4. Her ölçek iki yöntemle uygulanır:
   - INTER_NEAREST (en yakın komşu)
   - INTER_AREA (küçültme) / INTER_CUBIC (büyütme)

## Gözlemler
- Küçültmede en yakın komşu yöntemi kenarlarda tırtıklanma ve desen bozulması (aliasing) oluşturur. INTER_AREA pikselleri ortaladığı için daha düzgün sonuç verir.
- Büyütmede en yakın komşu yöntemi görüntüyü kare kare (pikselli) yapar. INTER_CUBIC daha yumuşak geçiş üretir ama kaybolan detayı geri getiremez.

## Gereksinimler
Visual Studio 2026, OpenCV 4.14.0