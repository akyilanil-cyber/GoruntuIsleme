# Hafta 3 - Görev 2: Elle Histogram Eşitleme

## Amaç
Düşük kontrastlı, tek kanallı bir görüntünün histogramını, kümülatif dağılımını (CDF) ve histogram eşitlemesini hazır fonksiyon kullanmadan hesaplamak.

## Yöntem
1. Görüntü gri seviyede (tek kanal) açılır.
2. 256 elemanlı histogram elle sayılır.
3. Histogramdan kümülatif dağılım (CDF) hesaplanır.
4. Dönüşüm: yeni = round((CDF[v] - CDFmin) / (N - CDFmin) × 255)
5. Tablo her piksele uygulanır, sonuç esitlenmis.png olarak kaydedilir.

calcHist ve equalizeHist kullanılmamıştır.

## Sonuçlar
- Görüntü: 1066 x 1600 (1.705.600 piksel)
- Parlaklık aralığı 0-237'den 0-255'e genişledi.
- Örnek: piksellerin %62,8'i 207 ve altında olduğu için 207 değeri 160'a dönüştü.
- Eşitlenmiş histogramdaki aralıklı görünüm, ayrık değerlerin geniş aralığa yayılmasından kaynaklanır.