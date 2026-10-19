# 7x7 LoG ile Kenar Tespiti: Gauss Uygulanmış ve Uygulanmamış Kenar Filtreleriyle Karşılaştırma

## 1. LoG çekirdeği
Laplacian of Gaussian, Gauss yumuşatması ile Laplacian'ın tek bir çekirdekte birleştirilmiş halidir:

LoG(x, y) = -1/(πσ⁴) · (1 − r²/2σ²) · e^(−r²/2σ²),  r² = x² + y²

Çekirdek σ = 1,0 ile 7x7 olarak üretilmiştir. 7x7 boyut merkezden ±3 piksel, yani ±3σ'yı kapsar; Gauss'un neredeyse tamamı çekirdeğin içinde kalır. Kesilmeden kaynaklanan küçük hatanın düz bölgelerde cevap üretmemesi için çekirdeğin ortalaması çıkarılarak toplamı 0 yapılmıştır. Merkez negatif (-0,318), çevresi pozitiftir ("Meksika şapkası" biçimi, log_cekirdek.png). Kenarlar, Ödev 5'teki gibi sıfır geçişlerinden bulunmuştur.

## 2. Karşılaştırılan yöntemler
| Yöntem | Ön yumuşatma |
|---|---|
| Prewitt, Sobel, Laplacian | Yok |
| Gauss + Prewitt, Gauss + Sobel, Gauss + Laplacian | 3x3 Gauss, σ = 0,8 |
| LoG 7x7 | Çekirdeğin içinde (σ = 1,0) |

## 3. Doğruluk (sentetik görüntü, 2 piksel tolerans, her yöntem için en iyi eşik)
| Yöntem | F1, σ = 5 | F1, σ = 12 | F1, σ = 25 | Yanlış kenar, σ = 25 |
|---|---|---|---|---|
| Prewitt | 1,000 | 1,000 | 0,900 | 163 |
| Gauss + Prewitt | 1,000 | 1,000 | **0,982** | 76 |
| Sobel | 1,000 | 0,999 | 0,901 | 118 |
| Gauss + Sobel | 1,000 | 1,000 | **0,981** | 30 |
| Laplacian | 0,989 | 0,711 | 0,301 | 959 |
| Gauss + Laplacian | 1,000 | 0,944 | 0,508 | 702 |
| LoG 7x7 | 1,000 | 0,970 | 0,653 | 507 |

## 4. Gerçek görüntü (868x600, eşikler σ = 5 durumundan)
| Yöntem | Kenar piksel oranı | Süre |
|---|---|---|
| Prewitt | %15,8 | 6,36 ms |
| Gauss + Prewitt | %21,9 | 6,37 ms |
| Sobel | %14,5 | 6,02 ms |
| Gauss + Sobel | %21,0 | 6,48 ms |
| Laplacian | %0,8 | 9,73 ms |
| Gauss + Laplacian | %10,1 | 9,15 ms |
| LoG 7x7 | %13,9 | 16,96 ms |

## 5. Yorum
**Gauss yumuşatmasının etkisi:** Tüm yöntemlerde yumuşatma doğruluğu artırmıştır. Etkinin büyüklüğü ise türevin derecesine bağlıdır. Gradyan yöntemlerinde σ = 25 gürültüde F1 0,90'dan 0,98'e çıkmıştır. Laplacian'da 0,30'dan 0,51'e çıkmış ama yine de yetersiz kalmıştır. İkinci türev gürültüyü çok daha fazla büyüttüğü için daha güçlü yumuşatmaya ihtiyaç duyar.

**LoG, ikinci türev yöntemlerinin en iyisidir.** Aynı gürültüde LoG F1 = 0,653 ile Gauss + Laplacian'ı (0,508) ve yalın Laplacian'ı (0,301) geçmiştir. Yanlış kenar sayısı da 959'dan 507'ye inmiştir. Bunun nedeni, 7x7 LoG'un içindeki yumuşatmanın (σ = 1,0, 7x7 alan) 3x3 Gauss'tan (σ = 0,8) daha güçlü olmasıdır. Ayrıca yumuşatma ve türev tek çekirdekte birleştiği için ara sonuçta yuvarlama da yoktur.

**Gradyan yöntemleri yine de daha dayanıklıdır.** Güçlü gürültüde Gauss + Sobel/Prewitt (0,98), LoG'dan (0,65) belirgin şekilde iyidir. LoG'un kalan yanlış kenarları, düz bölgelerde gürültünün oluşturduğu küçük sıfır geçişleridir. Bunlar daha büyük σ ile azaltılabilir, ancak bu durumda ince ayrıntılar ve köşeler de kaybolur.

**Gerçek görüntüde** yumuşatmasız Laplacian neredeyse hiç kenar vermemiştir (%0,8). Düşük gürültüde bile yanlış kenarları ayıklamak için gereken eşik o kadar yüksektir ki gerçek kenarların çoğu da elenmiştir. LoG ise yumuşatmasız Laplacian ile aynı yaklaşımı kullanmasına rağmen binanın pencere ve kat çizgilerini ince, kapalı konturlar halinde bulmuştur (kenar_LoG_7x7.png).

**Süre:** 7x7 çekirdek piksel başına 49 çarpma yapar, 3x3 çekirdek ise 9. Bu yüzden LoG en yavaş yöntemdir (17 ms). LoG, iki ayrılabilir (separable) çekirdeğin toplamı olarak yazılarak hızlandırılabilir.

## 6. Sonuç
- Kenar filtrelerinden önce Gauss uygulamak her yöntemde doğruluğu artırır. İkinci türev yöntemlerinde ise zorunludur.
- 7x7 LoG, Laplacian tabanlı yöntemler arasında gürültüye en dayanıklı olandır. İnce, kapalı ve yönden bağımsız kenarlar verir.
- Güçlü gürültüde Gauss + Sobel/Prewitt daha doğrudur ve yaklaşık 2,6 kat daha hızlıdır. LoG'un üstünlüğü, kenarı tek piksel kalınlığında ve kapalı kontur olarak vermesidir.
