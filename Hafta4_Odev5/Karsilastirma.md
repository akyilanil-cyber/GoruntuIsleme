# Laplacian, Sobel ve Prewitt ile Kenar Tespitinin Karşılaştırılması

## 1. Yöntemlerin farkı
| | Prewitt / Sobel | Laplacian |
|---|---|---|
| Türev | Birinci türev (gradyan) | İkinci türev |
| Çekirdek sayısı | 2 (X ve Y) | 1: [[0 1 0], [1 -4 1], [0 1 0]] |
| Kenar nerede? | Gradyan büyüklüğünün yüksek olduğu yerde | Cevabın işaret değiştirdiği yerde (sıfır geçişi) |
| Yön bilgisi | Var (Gx, Gy) | Yok, tüm yönlere eşit cevap verir (izotropik) |
| Kenar kalınlığı | Eşiğe göre kalın bir şerit | İnce çizgi (sıfır geçişi tek piksellik bir sınırdır) |

Laplacian bir kenarın iki yanında zıt işaretli iki tepe üretir. Kenar, bu iki tepe arasındaki sıfır geçişindedir. Bu projede bir pikselin karşılıklı iki komşusu (yatay, dikey veya çapraz) zıt işaretliyse o piksel kenar adayı sayılır. İki komşu arasındaki fark ise kenar gücü olarak eşiklenir. Tüm yöntemlerden önce görüntü 3x3 Gauss filtresiyle (sigma = 0,8) yumuşatılmıştır.

## 2. Ölçüm sonuçları
Kenarların yeri bilinen 400x400 sentetik görüntüye üç seviyede Gauss gürültüsü eklenmiştir. Her yöntem için F1'i en yüksek yapan eşik seçilmiştir (2 piksel tolerans).

| Gürültü | Prewitt F1 | Sobel F1 | Laplacian F1 | Laplacian yanlış kenar | Laplacian F1 (yumuşatmasız) |
|---|---|---|---|---|---|
| σ = 5 | 1,000 | 1,000 | 1,000 | 0 | 0,989 |
| σ = 12 | 1,000 | 1,000 | 0,944 | 125 | 0,711 |
| σ = 25 | 0,982 | 0,981 | 0,508 | 702 | 0,301 |

Gerçek görüntüde (868x600) eşikler düşük gürültülü (σ = 5) durumdan alınmıştır:

| | Prewitt | Sobel | Laplacian |
|---|---|---|---|
| Kenar piksel oranı | %21,9 | %21,0 | %10,1 |
| Süre (yumuşatma dahil) | 6,48 ms | 6,72 ms | 9,57 ms |

## 3. Yorum
**Gürültü hassasiyeti en büyük farktır.** Gürültü arttıkça Laplacian'ın doğruluğu hızla düşer (σ = 25'te F1 = 0,51). Gradyan operatörleri ise 0,98'de kalır. İkinci türev, birinci türevin gürültüsünü bir kez daha büyütür. Çekirdekteki ağırlıkların kareleri toplamı Laplacian'da 20'dir (1+1+1+1+16), Sobel'in tek yönünde ise 12'dir. Ayrıca gürültü düz bölgelerde de sürekli sıfır geçişi üretir. Bu yüzden σ = 25'te düz bölgelerde 702 yanlış kenar pikseli oluşmuştur (sentetik_Laplacian.png). Yumuşatma yapılmadığında durum daha da kötüdür (F1 = 0,30). Laplacian'ın Gauss olmadan kullanılmaması gerektiği açıkça görülür.

**İnce ve kapalı kenarlar:** Gerçek görüntüde Laplacian, gradyan yöntemlerinin yarısı kadar kenar pikseli işaretlemiştir (%10'a karşı %21). Bunun nedeni daha az kenar bulması değil, kenarları ince çizgiler olarak vermesidir. Gradyan büyüklüğünün eşiklenmesi ise kenar çevresinde 2-3 piksellik bir şerit bırakır. Sıfır geçişleri doğası gereği kapalı konturlar oluşturur. Bu, nesne sınırlarını bulmak için avantajlıdır.

**Yön bilgisi:** Sobel ve Prewitt kenarın yönünü de verir, Laplacian vermez. Yön gerektiren sonraki adımlar (non-maximum suppression, köşe bulma) için gradyan operatörleri gerekir.

**Hız:** Laplacian tek çekirdek kullanmasına rağmen daha yavaş çıkmıştır (9,6 ms). Fark, sıfır geçişi aramasının ek bir tam görüntü taramasından gelir. Sadece filtreleme süresi karşılaştırılsaydı Laplacian (1 konvolüsyon) daha hızlı olurdu.

## 4. Sonuç
- Düşük gürültülü görüntülerde üç yöntem de kenarları doğru bulur. Laplacian daha ince ve kapalı kenarlar verir.
- Gürültü arttıkça Laplacian hızla bozulur. Bu nedenle pratikte önce Gauss ile yumuşatılarak, yani LoG (Laplacian of Gaussian) olarak kullanılır (Ödev 6).
- Sobel ve Prewitt gürültüye karşı çok daha dayanıklıdır ve yön bilgisi de verir. Genel amaçlı kenar bulmada daha güvenilirdir.
