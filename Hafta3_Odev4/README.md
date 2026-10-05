# Hafta 3 - Görev 4: 3x3 Ortalama Filtresi ile Gürültü Bastırma

## Amaç
Gürültülü bir resim üzerinde 3x3 ortalama filtresini konvolüsyon ile gezdirerek gürültüyü bastırmak.

## Yöntem
1. Görüntüye Gauss gürültüsü (sigma = 25) eklenir.
2. Tüm elemanları 1/9 olan 3x3 çekirdek, her pikselin üzerine konularak komşularla çarpılıp toplanır (elle konvolüsyon).
3. Kenarlarda görüntü aynalanır (reflect).
4. Sonuç OpenCV'nin blur fonksiyonu ve PSNR ölçümüyle test edilir.

## Sonuçlar
- OpenCV blur ile en büyük fark: 0 (elle konvolüsyon doğru)
- PSNR gürültülü: 20,61 dB
- PSNR filtreli: 29,93 dB
- Filtre gürültüyü belirgin şekilde azaltmıştır. Bedeli, kenarların bir miktar yumuşamasıdır.