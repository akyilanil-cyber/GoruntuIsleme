# Görüntü İşleme Dersi Ödevleri

Görüntü İşleme dersi haftalık ödevleri (C++ / OpenCV). Her ödev ayrı bir klasörde, ayrı bir Visual Studio projesidir. Her klasörde kaynak kod, ödevin amacını, yöntemini ve sonuçlarını anlatan bir README ve çıktı görüntüleri bulunur.

## Hafta 2
| Klasör | Konu |
|---|---|
| [Hafta2_Odev1](Hafta2_Odev1) | Farklı çözünürlüklerde yeniden boyutlandırma |
| [Hafta2_Odev2](Hafta2_Odev2) | K-Means ile renk nicemleme |
| [Hafta2_Odev3](Hafta2_Odev3) | Parçalı histogram (4 thread) |
| [Hafta2_Odev4](Hafta2_Odev4) | Doğrusal parlaklık / kontrast dönüşümü |

## Hafta 3
| Klasör | Konu |
|---|---|
| [Hafta3_Odev2](Hafta3_Odev2) | Elle histogram eşitleme |
| [Hafta3_Odev3](Hafta3_Odev3) | CLAHE |
| [Hafta3_Odev4](Hafta3_Odev4) | 3x3 ortalama filtresi ile gürültü bastırma |

## Hafta 4 - Kenar Tespiti
| Klasör | Konu |
|---|---|
| [Hafta4_Odev1](Hafta4_Odev1) | Prewitt [-1 0 1] filtresi (Gauss yumuşatma ile) |
| [Hafta4_Odev2](Hafta4_Odev2) | Prewitt [-1 -1 -1; 0 0 0; 1 1 1] filtresi |
| [Hafta4_Odev3](Hafta4_Odev3) | Yatay ve dikey Sobel filtreleri |
| [Hafta4_Odev4](Hafta4_Odev4) | Sobel ve Prewitt karşılaştırması ([Karsilastirma.md](Hafta4_Odev4/Karsilastirma.md)) |
| [Hafta4_Odev5](Hafta4_Odev5) | 3x3 Laplacian ile kenar tespiti ([Karsilastirma.md](Hafta4_Odev5/Karsilastirma.md)) |
| [Hafta4_Odev6](Hafta4_Odev6) | 7x7 Laplacian of Gaussian ([Rapor.md](Hafta4_Odev6/Rapor.md)) |
| [Hafta4_Odev7](Hafta4_Odev7) | OpenCV Canny, süre ve doğruluk karşılaştırması |
| [Hafta4_Canny](Hafta4_Canny) | Haftalık ödev: Canny'nin sıfırdan yazılması ([Canny_Adimlari.md](Hafta4_Canny/Canny_Adimlari.md)) |
| [Hafta4_Colab_CUDA](Hafta4_Colab_CUDA) | Google Colab'da CUDA destekli OpenCV kurulumu |
| [Hafta4_Boyutlandirma](Hafta4_Boyutlandirma) | Dizindeki görüntüleri 1024x768'e boyutlandırma |
| [Hafta4_Ortak](Hafta4_Ortak) | Hafta 4 projelerinin ortak kullandığı fonksiyonlar |

## Hafta 5 - Renk Uzayları
| Klasör | Konu |
|---|---|
| [Hafta5_Odev1](Hafta5_Odev1) | Kanal histogramları (calcHist) |
| [Hafta5_Odev2](Hafta5_Odev2) | Kendi calcHist fonksiyonu |
| [Hafta5_Odev3](Hafta5_Odev3) | RGB → gri dönüşümü ve hata değeri |
| [Hafta5_Odev4](Hafta5_Odev4) | Renk uzayı dönüşümleri (fonksiyon pointer'ları ile) |
| [Hafta5_CUDA](Hafta5_CUDA) | Haftalık ödev: tüm görevlerin CUDA ile gerçekleştirilmesi |

## Çalıştırma
1. `GoruntuIsleme.slnx` Visual Studio ile açılır.
2. Çalıştırılacak proje "Başlangıç projesi olarak ayarla" ile seçilir, platform **x64** olmalıdır.
3. Programlar görüntüyü kendi klasöründen okur (`resim.jpg`). Çalışma dizini proje klasörüdür.

CUDA projeleri (`Hafta5_CUDA`, `Hafta4_Colab_CUDA`) NVIDIA ekran kartı gerektirir. Bu yüzden çözüme eklenmemiştir; derleme adımları kendi README dosyalarındadır.

## Gereksinimler
Visual Studio 2026, OpenCV 4.14.0 (`C:\opencv`)
