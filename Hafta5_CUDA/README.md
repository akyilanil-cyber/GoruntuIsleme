# Hafta 5 - Haftalık Ödev: Görevlerin CUDA ile Gerçekleştirilmesi

## Amaç
Ödev 1–4'teki tüm görevleri CUDA kullanarak GPU üzerinde yeniden gerçekleştirmek.

## Yöntem
1. **Histogram (Ödev 1-2):** Her blok 256 kutulu histogramını paylaşımlı bellekte (`__shared__`) tutar ve `atomicAdd` ile sayar. Blok sonunda sonuçlar global histograma eklenir. Her kanal için ayrı çalıştırılır ve `calcHist` ile karşılaştırılır.
2. **Gri dönüşüm (Ödev 3):** Her thread bir pikseli işler ve aynı tam sayı katsayıları (9798, 19235, 3735) kullanır.
3. **Renk uzayları (Ödev 4):** GRAY, RGB, HSV, YCrCb ve XYZ dönüşümleri için piksel başına `__device__` fonksiyonları yazılmıştır. Ortak bir 2B çekirdek (16x16 blok) hangi dönüşümü yapacağını şablon parametresinden alır. Host tarafında dönüşümler yine bir fonksiyon pointer tablosu üzerinden çağrılır.
4. HSV bölme tabloları CPU'da hesaplanıp GPU'nun sabit belleğine (`__constant__`) kopyalanır.
5. Her GPU sonucu OpenCV'nin CPU fonksiyonlarıyla karşılaştırılır. Çekirdek süreleri `cudaEvent` ile ölçülür.

## Derleme
```
nvcc -O2 -std=c++17 Hafta5_CUDA.cu -o Hafta5_CUDA.exe -I C:\opencv\build\include -L C:\opencv\build\x64\vc16\lib -lopencv_world4140
```
veya `derle_cuda.bat`.

## Gereksinimler
NVIDIA ekran kartı, CUDA Toolkit, Visual Studio 2026, OpenCV 4.14.0
