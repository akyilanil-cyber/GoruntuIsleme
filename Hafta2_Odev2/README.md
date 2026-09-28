# Hafta 2 - Ödev 2: K-Means ile Renk Nicemleme

## Amaç
Bir görüntüdeki renk sayısını k-means kümeleme ile k renge (4, 8, 16) indirmek.

## Yöntem
1. Görüntü BGR'den Lab renk uzayına çevrilir. Lab uzayında iki renk arasındaki öklid mesafesi, insan gözünün algıladığı farka daha yakındır.
2. Görüntü (yükseklik × genişlik × 3) şeklinden (piksel sayısı × 3) şeklinde bir tabloya dönüştürülür.
3. OpenCV'nin cv::kmeans fonksiyonu ile pikseller k kümeye ayrılır (k-means++ başlangıcı, 3 deneme).
4. Her piksel kendi kümesinin merkez rengiyle değiştirilir, görüntü tekrar BGR'ye çevrilir.

## Gözlemler
- k=4'te görüntü poster/çizgi film görünümüne benzer, birçok renk detayı kaybolur.
- k büyüdükçe görüntü orijinaline yaklaşır; k=16'da genel görünüm büyük ölçüde korunur.
- k arttıkça hesaplama süresi de artar.

## Kaynak
pyimagesearch.com - Color Quantization with OpenCV using K-Means Clustering (Python örneği C++'a uyarlandı)

## Gereksinimler
Visual Studio 2026, OpenCV 4.14.0