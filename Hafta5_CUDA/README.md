# Hafta 5 - Haftalık Ödev: Görevlerin CUDA ile Gerçekleştirilmesi

## Amaç
Ödev 1–4'teki tüm görevleri CUDA kullanarak GPU üzerinde yeniden gerçekleştirmek.

## Yöntem
1. **Histogram (Ödev 1-2):** Her blok 256 kutulu histogramını paylaşımlı bellekte (`__shared__`) tutar ve `atomicAdd` ile sayar. Blok sonunda sonuçlar global histograma eklenir. Her kanal için ayrı çalıştırılır ve `calcHist` ile karşılaştırılır.
2. **Gri dönüşüm (Ödev 3):** Her thread bir pikseli işler ve aynı tam sayı katsayıları (9798, 19235, 3735) kullanır.
3. **Renk uzayları (Ödev 4):** GRAY, RGB, HSV, YCrCb ve XYZ dönüşümlerinin her biri, piksel başına çalışan bir `__device__` fonksiyon nesnesidir (functor). Ortak bir 2B çekirdek (16x16 blok) hangi dönüşümü yapacağını şablon parametresinden alır. Host tarafında dönüşümler yine bir fonksiyon pointer tablosu üzerinden çağrılır.
4. HSV bölme tabloları CPU'da hesaplanıp GPU'nun sabit belleğine (`__constant__`) kopyalanır.
5. Her GPU sonucu OpenCV'nin CPU fonksiyonlarıyla karşılaştırılır. Çekirdek süreleri `cudaEvent` ile ölçülür.

## Sonuçlar
Google Colab, Tesla T4 GPU, CUDA 13.0, OpenCV 4.6.0, resim.jpg (612 x 416)

| Görev | OpenCV ile mutlak fark toplamı | Farklı piksel | Çekirdek süresi |
|---|---|---|---|
| Histogram B / G / R | 0 / 0 / 0 | | |
| BGR → GRAY | 0 | 0 | 0,026 ms |
| BGR → RGB | 0 | 0 | 0,059 ms |
| BGR → HSV | 0 | 0 | 0,074 ms |
| BGR → YCrCb | 0 | 0 | 0,048 ms |
| BGR → XYZ | 0 | 0 | 0,045 ms |

- Beklenen hata değeri olan 0 tüm görevlerde GPU üzerinde de elde edilmiştir.
- GPU çekirdekleri 254592 pikseli 0,1 ms'nin altında işler. Toplam sürenin büyük kısmı CPU-GPU arası veri kopyalamadır.

## Derleme ve çalıştırma

**Google Colab (NVIDIA kartı olmayan bilgisayarlar için):** Çalışma zamanı → T4 GPU seçilir ve şu hücre çalıştırılır:
```
!apt-get -qq update > /dev/null; apt-get -qq install -y libopencv-dev > /dev/null
!wget -q https://raw.githubusercontent.com/akyilanil-cyber/GoruntuIsleme/master/Hafta5_CUDA/Hafta5_CUDA.cu https://raw.githubusercontent.com/akyilanil-cyber/GoruntuIsleme/master/Hafta5_CUDA/resim.jpg
!nvcc -O2 -std=c++17 Hafta5_CUDA.cu -o h5 `pkg-config --cflags --libs opencv4` && ./h5 resim.jpg
```

**Windows (NVIDIA kartı ve CUDA Toolkit kurulu):**
```
nvcc -O2 -std=c++17 Hafta5_CUDA.cu -o Hafta5_CUDA.exe -I C:\opencv\build\include -L C:\opencv\build\x64\vc16\lib -lopencv_world4140
```
veya `derle_cuda.bat`.

## Gereksinimler
NVIDIA ekran kartı (veya Google Colab T4), CUDA Toolkit, OpenCV 4.x
