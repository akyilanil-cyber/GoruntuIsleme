# Hafta 5 - Ödev 3: RGB → Gri Dönüşümü

## Amaç
RGB görüntüyü tek kanallı gri tonlu görüntüye dönüştürmek. Dönüşümü hem OpenCV'nin hazır fonksiyonuyla hem kendi fonksiyonumuzla yapıp, eşleşen piksellerin mutlak farklarının toplamını hata değeri olarak raporlamak.

## Yöntem
1. Gri = 0,299 R + 0,587 G + 0,114 B
2. OpenCV 8 bitlik görüntülerde bu katsayıları 2^15 = 32768 ile çarpılmış tam sayılar olarak kullanır: 9798, 19235, 3735 (toplamları tam 32768).
3. Kendi fonksiyonumuz aynı tam sayı katsayıları kullanır, 2^14 ekleyerek yuvarlar ve 15 bit sağa kaydırır (32768'e böler).
4. Sonuç `cvtColor(..., COLOR_BGR2GRAY)` ile karşılaştırılır: hata = Σ |OpenCV − kendi|.
5. Karşılaştırma için aynı formülün ondalıklı (double) hali de denenmiştir.

## Sonuçlar
| Yöntem | Hata (mutlak fark toplamı) | Farklı piksel |
|---|---|---|
| Kendi fonksiyon (tam sayı katsayılar) | 0 | 0 |
| Ondalıklı formül | 192 | 192 |

- Kendi fonksiyonumuz OpenCV ile birebir aynı sonucu verir (hata = 0).
- Ondalıklı formülde 192 piksel 1 farklı çıkar. Sonucun tam x,5'e çok yakın düştüğü piksellerde, tam sayı katsayılardaki küçük yuvarlama farkı sonucu bir üst ya da alt değere taşır.
- Çıktılar gri_opencv.png ve gri_kendi.png olarak kaydedilmiştir.

## Gereksinimler
Visual Studio 2026, OpenCV 4.14.0
