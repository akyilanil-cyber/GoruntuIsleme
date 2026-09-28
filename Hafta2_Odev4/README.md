# Hafta 2 - Ödev 4: Doğrusal Parlaklık/Kontrast Dönüşümü

## Amaç
Her pikselin değerini 0.75 ile çarpıp 20 eklemek: yeni = 0.75 × eski + 20

## Yöntem
1. Görüntü 4 çeyreğe bölünür, her çeyrek ayrı bir thread ve ayrı bir CPU çekirdeğinde işlenir.
2. Her pikselin her renk kanalına yeni = 0.75 × eski + 20 uygulanır.
3. saturate_cast ile sonuç yuvarlanır ve 0-255 aralığında tutulur.
4. Sonuç OpenCV'nin convertTo fonksiyonuyla karşılaştırılarak doğrulanır.

## Gözlemler
| Eski | Yeni |
|---|---|
| 0 | 20 |
| 50 | 58 |
| 100 | 95 |
| 200 | 170 |
| 255 | 211 |

- 0.75 ile çarpmak kontrastı düşürür: değerler birbirine yaklaşır, en parlak nokta 255'ten 211'e iner.
- +20 parlaklığı artırır: tam siyah (0) artık 20 olur.
- Sonuçta tüm değerler 20-211 aralığına sıkışır, görüntü daha soluk ve gri tonlu görünür.

## Gereksinimler
Visual Studio 2026, OpenCV 4.14.0