@echo off
set BUILD_TYPE="RELEASE"

:: Compile shaders
for %%F in (shaders/*) do (
  glslc "src\shaders\%%F" -o "products\assets\shaders\%%F.spv"
)

:: Compile code and link libraries (-mwindows for no console)
mkdir build
cd build
cmake ^
 -G "MinGW Makefiles" ^
 -DCMAKE_CXX_COMPILER=g++ ^
 ..
mingw32-make

:: Copy all outputs to products
for /R . %%f in (*.dll) do copy %%f "..\products\bin\"
copy "game.exe" "..\products\bin\"
copy "host.exe" "..\products\bin\"
copy "launcher.exe" "..\products\"

:: Copy all depedencies (if required)
::cd "..\lib\"
::for /R . %%f in (*.dll) do copy %%f "..\products\bin\"

PAUSE
