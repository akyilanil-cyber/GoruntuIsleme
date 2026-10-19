@echo off
rem CUDA surumu: NVIDIA ekran karti + CUDA Toolkit + Visual Studio gerektirir
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
cd /d "%~dp0"
nvcc -O2 -std=c++17 Hafta5_CUDA.cu -o Hafta5_CUDA.exe -I C:\opencv\build\include -L C:\opencv\build\x64\vc16\lib -lopencv_world4140
if errorlevel 1 exit /b 1
copy /y C:\opencv\build\x64\vc16\bin\opencv_world4140.dll . >nul
.\Hafta5_CUDA.exe resim.jpg
