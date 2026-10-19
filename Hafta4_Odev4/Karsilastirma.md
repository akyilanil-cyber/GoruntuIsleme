# Sobel ve Prewitt Operatörlerinin Karşılaştırılması

## 1. İki operatör arasındaki fark
İki operatör de 3x3 boyutunda, birinci türeve dayalı gradyan operatörüdür. Her ikisi de türev yönünde [-1 0 1] farkını alır. Fark, türeve dik yöndeki ağırlıklardadır:

| | Dik yöndeki ağırlıklar | Ağırlık toplamı |
|---|---|---|
| Prewitt | [1 1 1] (düz ortalama) | 3 |
| Sobel | [1 2 1] (merkeze ağırlık veren yumuşatma) | 4 |

Sobel, merkezdeki satıra/sütuna iki kat ağırlık verir. Yani türevi alırken küçük bir Gauss benzeri yumuşatma da yapar. Prewitt ise üç satırı eşit ağırlıkla ortalar. Adil bir karşılaştırma için sonuçlar ağırlık toplamına bölünmüştür (Prewitt / 3, Sobel / 4). Böylece ikisi de 0-255 aralığında, aynı ölçekte ifade edilir. Her iki operatörden önce görüntü 3x3 Gauss filtresiyle (sigma = 0,8) yumuşatılmıştır.

## 2. Ölçüm sonuçları
Sayılar `Hafta4_Odev4.cpp` ile ölçülmüştür (Release x64, 868x600 building.jpg ve 400x400 sentetik test görüntüsü).

| Ölçüt | Prewitt | Sobel |
|---|---|---|
| Gerçek görüntü, ortalama gradyan büyüklüğü | 16,01 | 16,13 |
| Gerçek görüntü, kenar pikseli (> 40) | 63622 | 64068 |
| İki sonuç arasındaki ortalama / en büyük fark | 0,24 / 6,96 | |
| Gürültü σ=12: düz bölge cevabı / kenar cevabı | 6,18 / 50,71 (oran 8,21) | 6,46 / 51,23 (oran 7,93) |
| Gürültü σ=12: F1 | 1,000 | 1,000 |
| Gürültü σ=25: düz bölge cevabı / kenar cevabı | 12,79 / 51,74 (oran 4,04) | 13,39 / 52,35 (oran 3,91) |
| Gürültü σ=25: F1 (kesinlik / duyarlılık) | 0,982 (0,974 / 0,989) | 0,981 (0,988 / 0,973) |
| Daire kenarında ortalama yön hatası | 1,56° | 1,08° |
| Çalışma süresi (yumuşatma dahil) | 6,12 ms | 6,26 ms |

## 3. Yorum
**Görsel sonuç neredeyse aynıdır.** Gerçek görüntüde iki büyüklük haritası arasındaki ortalama fark 0-255 ölçeğinde 0,24 gri seviyesidir. Normalize korelasyon 1,000'dir. İki kenar haritasını gözle ayırt etmek zordur (fark.png).

**Gürültü:** Beklentinin aksine, ölçümde Prewitt'in sinyal/gürültü oranı biraz daha yüksek çıkmıştır (8,21'e karşı 7,93). Bunun nedeni matematikseldir. Bağımsız gürültünün eşit ağırlıklı ortalaması, en düşük varyansı veren ortalamadır. Normalize ağırlıkların karelerinin toplamı Prewitt için 3/9 = 0,333, Sobel için 6/16 = 0,375'tir. Yani Prewitt gürültüyü teoride de biraz daha fazla bastırır. Güçlü gürültüde (σ = 25) iki operatörün F1 değeri neredeyse eşittir (0,982 ve 0,981). Sobel daha az yanlış kenar bulur (kesinlik 0,988). Prewitt ise gerçek kenarları biraz daha eksiksiz yakalar (duyarlılık 0,989).

**Yön doğruluğu (Sobel'in asıl üstünlüğü):** Bir dairenin kenarında ölçülen gradyan yönü ile gerçek yön arasındaki ortalama hata Sobel'de 1,08°, Prewitt'te 1,56°'dir. Sobel'in merkeze ağırlık veren yapısı, operatörün tüm yönlere daha eşit (izotropik) cevap vermesini sağlar. Prewitt ise çapraz kenarları yatay/dikey kenarlardan farklı güçte görür. Gradyan yönünün kullanıldığı işlemlerde bu fark önemlidir; örneğin Canny'deki non-maximum suppression ya da yön histogramları (HOG).

**Hız:** İki çekirdek de 3x3 olduğu için hesap maliyeti aynıdır. Ölçülen fark (6,12 ve 6,26 ms) ölçüm belirsizliği içindedir.

## 4. Sonuç
- Sadece "kenar var mı" sorusu için iki operatör pratikte aynı sonucu verir.
- Gürültü bastırmada Prewitt teoride ve ölçümde çok az önde çıkmıştır. Ancak fark, öncesinde uygulanan Gauss yumuşatmasının etkisinin yanında küçük kalır.
- Kenar yönünün önemli olduğu uygulamalarda Sobel tercih edilmelidir. Canny gibi algoritmaların gradyan adımında Sobel kullanılmasının nedeni budur.
