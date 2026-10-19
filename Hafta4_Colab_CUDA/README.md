# Hafta 4 - Google Colab Üzerinde CUDA Destekli OpenCV Kurulumu

## Amaç
Google Colab üzerinde CUDA destekli OpenCV kurmak ve C++ ile GPU üzerinde çalıştığını göstermek.

**Defter: [OpenCV_CUDA_Kurulum.ipynb](OpenCV_CUDA_Kurulum.ipynb)** (Colab'da açmak için: Dosya → Not defteri yükle veya GitHub sekmesinden bu depo)

## Neden kaynak koddan derleniyor?
Colab'daki hazır `opencv-python` paketi CUDA desteği olmadan derlenmiştir (`cv2.cuda.getCudaEnabledDeviceCount()` 0 döner). CUDA modülleri (`cudaarithm`, `cudaimgproc`, `cudawarping`, `cudafilters`) `opencv_contrib` deposundadır. Bu yüzden OpenCV ve opencv_contrib birlikte, CUDA açık olarak derlenir.

## Adımlar
1. Çalışma zamanı türü **T4 GPU** seçilir. `nvidia-smi` ve `nvcc --version` ile GPU ve CUDA sürümü kontrol edilir.
2. Derleme araçları kurulur (cmake, build-essential, resim kütüphaneleri).
3. OpenCV 4.14.0 ve opencv_contrib 4.14.0 indirilir (iki deponun sürümü aynı olmalıdır).
4. CMake ayarları:
   - `WITH_CUDA=ON`: CUDA desteği
   - `CUDA_ARCH_BIN=7.5`: yalnızca T4'ün mimarisi için derler, süreyi kısaltır
   - `BUILD_LIST=core,imgproc,imgcodecs,cudev,cudaarithm,cudaimgproc,cudawarping,cudafilters`: sadece gereken modüller derlenir (tam derleme 1,5 saati aşabilir)
   - `OPENCV_GENERATE_PKGCONFIG=ON`: C++ programı `pkg-config` ile derlenebilir
5. `make -j$(nproc)` ve `make install` (ücretsiz Colab T4, 2 çekirdek: yaklaşık 35-40 dakika)
6. İsteğe bağlı: Kurulum Google Drive'a arşivlenir. Sonraki oturumlarda derleme yapmadan geri yüklenir.
7. Test: C++ programı (`cuda_boyutlandir.cpp`) bir dizindeki görüntüleri `cv::cuda::GpuMat` ile GPU'ya yükler, `cv::cuda::resize` ile 1024x768'e boyutlandırır ve sonucu CPU `resize` ile karşılaştırır.

## Kontrol
- CMake çıktısında **NVIDIA CUDA: YES** satırı görünmelidir.
- Test programı `CUDA destekli GPU sayisi: 1` ve T4 bilgilerini yazdırmalıdır.

## Sonuçlar
Google Colab, Tesla T4, CUDA 13.0, OpenCV 4.14.0 (kaynak koddan derlendi). CMake çıktısı: `NVIDIA CUDA: YES (ver 13.0, CUFFT CUBLAS)`, `NVIDIA GPU arch: 75`

| Görüntü | Boyut | GPU | CPU | Fark: en büyük / ortalama |
|---|---|---|---|---|
| baboon.jpg | 512x512 → 1024x768 | 174,4 ms* | 3,67 ms | 50 / 4,79 |
| fruits.jpg | 512x480 → 1024x768 | 1,85 ms | 2,10 ms | 48 / 1,62 |
| lena.jpg | 512x512 → 1024x768 | 0,92 ms | 2,10 ms | 45 / 1,78 |
| building.jpg | 868x600 → 1024x768 | 1,44 ms | 2,18 ms | 18 / 1,00 |

\* İlk görüntüde CUDA bağlamı başlatıldığı için GPU süresi uzundur.

- GPU süresine CPU → GPU yükleme ve GPU → CPU indirme dahildir. Bu boyuttaki görüntülerde GPU, kopyalama maliyetine rağmen CPU'dan biraz daha hızlıdır.
- GPU ve CPU `resize` sonuçları birebir aynı değildir, çünkü iki uygulama ara değerlemeyi farklı yöntemlerle hesaplar. Ortalama fark 1-5 gri seviyedir. En büyük fark, keskin kenarlı ve dokulu bölgelerde oluşur (en fazla baboon görüntüsündeki tüylerde).

## Gereksinimler
Google Colab (T4 GPU çalışma zamanı)
