# Canny Kenar Bulma Algoritmasının Adımları Neden Gerekli?

Canny algoritması dört adımdan oluşur: gürültü bastırma, gradyan hesaplama, non-maximum suppression ve histerezis eşikleme. Canny bu algoritmayı tasarlarken iyi bir kenar bulucudan üç şey beklemiştir:

1. **İyi tespit:** Gerçek kenarlar kaçırılmamalı, gürültü kenar sanılmamalı.
2. **İyi konum:** Bulunan kenar, gerçek kenarın tam üzerinde olmalı.
3. **Tek cevap:** Her kenar için tek bir çizgi (tek piksel kalınlığında) bulunmalı.

Aşağıda her adımın bu hedeflerden hangisini sağladığı tartışılmış, ardından iddialar deneyle sınanmıştır.

## Deney düzeni
Adımların etkisini görmek için `Hafta4_Canny.cpp` içindeki `adimDeneyi` fonksiyonu, kenarlarının yeri bilinen 400x400'lük sentetik bir görüntü kullanır (dikdörtgen, daire, üçgen ve döndürülmüş dikdörtgen; 4 kapalı kenar). Görüntüye üç seviyede Gauss gürültüsü eklenir. Her denemede bir adım çıkarılır, kalan adımlar aynen çalıştırılır.

Her sürüm için F1'i en yüksek yapan eşik ayrıca aranmıştır (alt eşik = üst eşik / 2). Böylece sonuçlardaki fark kötü seçilmiş bir eşikten değil, adımın kendisinden kaynaklanır. Ölçütler:
- **F1:** 2 piksel toleransla doğruluk (kesinlik ve duyarlılığın birleşimi)
- **Kalınlık:** Bulunan kenar pikseli / gerçek kenar uzunluğu (ideal değer 1 piksel)
- **Parça:** Sonuçtaki birbirine bağlı kenar parçası sayısı (ideal değer 4, şekil sayısı kadar)

| Sürüm | F1 (σ=25) | Parça (σ=25) | F1 (σ=40) | Kalınlık (σ=40) | Parça (σ=40) |
|---|---|---|---|---|---|
| **Tam Canny (4 adım)** | **1,000** | **4** | **0,991** | **1,00 px** | **10** |
| 1. adım yok (Gauss yok) | 0,940 | 56 | 0,691 | 1,07 px | 132 |
| 3. adım yok (NMS yok) | 1,000 | 4 | 0,979 | 3,10 px | 6 |
| 4. adım yok (tek eşik) | 0,999 | 13 | 0,952 | 0,81 px | 97 |
| Sadece Gauss + Sobel + eşik | 1,000 | 15 | 0,967 | 2,19 px | 188 |

Hafif gürültüde (σ = 12) tüm sürümler F1 = 1,000 verir. Fark ancak gürültü arttıkça ortaya çıkar. Bu da adımların asıl görevinin gürültülü, gerçek görüntülerde sağlamlık kazandırmak olduğunu gösterir. σ = 40 için çıktılar `deney_*.png` dosyalarındadır.

## 1. Gürültü bastırma (5x5 Gauss filtresi)
**Neden gerekli:** Kenar bulma bir türev işlemidir ve türev, yüksek frekanslı değişimleri büyütür. Gürültü de tam olarak pikselden piksele hızlı değişen, yüksek frekanslı bir sinyaldir. Yumuşatma yapılmazsa her gürültü pikseli küçük bir "kenar" gibi güçlü bir gradyan üretir.

**Deneyde:** Gauss adımı çıkarıldığında σ = 40'ta F1 0,991'den 0,691'e düşmüş, kenar parça sayısı 10'dan 132'ye çıkmıştır. `deney_gauss_yok.png` dosyasında her yere dağılmış gürültü noktaları görülür. Kontrastı düşük daire ve döndürülmüş dikdörtgen ise neredeyse tamamen kaybolmuştur. Bunun nedeni, gürültünün eşiği yukarı çekmeye zorlamasıdır: Eşik, gürültüyü atacak kadar yükseltildiğinde zayıf gerçek kenarlar da atılır. Bu adım "iyi tespit" hedefini sağlar.

**Bedeli:** Fazla yumuşatma kenarları da bulanıklaştırır ve yakın kenarları birleştirir. σ, gürültüyü bastıracak kadar büyük, ince ayrıntıları yok etmeyecek kadar küçük seçilmelidir (bu çalışmada σ = 1,4).

## 2. Sobel ile gradyan hesaplama
**Neden gerekli:** Kenar, parlaklığın hızlı değiştiği yerdir. Bu değişimi ölçen büyüklük gradyandır. Sobel X ve Y çekirdekleri iki yöndeki türevi verir:
- **Büyüklük** √(Gx² + Gy²): kenarın ne kadar güçlü olduğunu söyler.
- **Yön** atan2(Gy, Gx): kenarın hangi yöne baktığını söyler.

Sonraki iki adım bu iki bilgiye dayanır. Non-maximum suppression yönü, histerezis ise büyüklüğü kullanır. Sobel'in merkeze 2 kat ağırlık vermesi, gradyan yönünü Prewitt'e göre daha doğru hesaplamasını sağlar (Hafta4_Odev4'teki ölçümde daire üzerindeki ortalama yön hatası Sobel'de 1,08°, Prewitt'te 1,56°). NMS doğru komşuları seçebilmek için bu yöne ihtiyaç duyar.

**Tek başına yetmez:** Gerçek görüntüde (868x600) gradyan 520800 pikselin 518321'inde sıfırdan büyüktür. Yani gradyan bir "kenar haritası" değil, her yerde değeri olan bir "kenar gücü haritası"dır. Bu haritayı tek eşikle kenara çevirmek (tablodaki son satır) kalın ve kopuk kenarlar verir: σ = 40'ta 2,19 piksel kalınlık ve 188 parça.

## 3. Non-maximum suppression (NMS)
**Neden gerekli:** Gauss yumuşatmasından sonra keskin bir kenar, gradyan haritasında birkaç piksel genişliğinde bir tepe hâline gelir. Bu tepeyi eşiklemek kalın bir şerit üretir. Şeridin tam ortası dışındaki pikseller aynı kenarın tekrarıdır. NMS her pikseli gradyan yönündeki iki komşusuyla karşılaştırır ve yerel en büyük olmayanları siler. Böylece tepenin sadece sırtı kalır. Bu adım "tek cevap" ve "iyi konum" hedeflerini sağlar.

**Deneyde:** NMS çıkarıldığında kenar kalınlığı 1,00 pikselden 3,10 piksele çıkmıştır (`deney_nms_yok.png`). Gerçek görüntüde NMS, aday kenar pikseli sayısını 518321'den 118616'ya indirir.

**Neden yön önemli:** Karşılaştırma kenar boyunca değil, kenara dik yönde (gradyan yönünde) yapılmalıdır. Kenar boyunca komşular aynı kenara aittir; onlarla karşılaştırma yapılsaydı kenar çizgisi kesik kesik olurdu. Bu yüzden yön 4 sınıfa (0°, 45°, 90°, 135°) yuvarlanır ve o yöndeki iki komşu seçilir.

## 4. Histerezis eşikleme
**Neden gerekli:** NMS'den sonra bile her sırtta bir değer vardır, gürültünün oluşturduğu sırtlarda da. Tek bir eşik seçmek iki kötü seçenek arasında kalmak demektir:
- Eşik yüksek seçilirse gürültü atılır, ama kenar boyunca gücü eşiğin biraz altına düşen yerlerde kenar kopar.
- Eşik düşük seçilirse kenarlar bütün kalır, ama gürültü noktaları da kenar sayılır.

Histerezis iki eşik kullanır. Üst eşiği geçen pikseller kesin kenardır. İki eşik arasındaki pikseller ancak bir kesin kenara bağlıysa kenar sayılır. Bunun dayandığı fikir şudur: Gerçek kenarlar süreklidir, gürültü ise dağınık ve bağlantısızdır. Bu adım hem "iyi tespit" hem de kenar sürekliliğini sağlar.

**Deneyde:** Histerezis yerine tek eşik kullanıldığında, en iyi eşik seçildiği hâlde σ = 40'ta kenar parça sayısı 10'dan 97'ye çıkmış, F1 0,991'den 0,952'ye düşmüştür. `deney_tek_esik.png` dosyasında kontrastı düşük daire ve döndürülmüş dikdörtgen kesik çizgi hâline gelmiştir. Tam Canny'de aynı şekiller kapalı çizgi olarak kalır. Duyarlılık da 1,000'den 0,934'e düşmüştür; yani kopan yerlerde gerçek kenar kaybedilmiştir.

## Sonuç
| Adım | Çözdüğü sorun | Çıkarılınca (σ = 40) |
|---|---|---|
| 1. Gauss | Gürültünün türevle büyümesi | F1 0,991 → 0,691, parça 10 → 132 |
| 2. Sobel | Kenar gücü ve yönünün ölçülmesi | Diğer adımlar için girdi yoktur |
| 3. NMS | Kalın kenar, aynı kenarın tekrarı | Kalınlık 1,00 → 3,10 piksel |
| 4. Histerezis | Tek eşiğin kopukluk / gürültü ikilemi | Parça 10 → 97, F1 0,991 → 0,952 |

Adımlar birbirini tamamlar. Gauss, Sobel'in gürültüyü kenar sanmasını önler. Sobel, NMS'in ihtiyaç duyduğu yönü verir. NMS kenarı tek piksele indirir. Histerezis bu ince çizgileri koparmadan ve gürültüyü almadan seçer. Herhangi bir adım çıkarıldığında, gürültü arttıkça sonuç belirgin şekilde bozulmaktadır.
