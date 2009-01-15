
@set build_dir=%1
@if "%2"=="generator_nmake" set generator_name=NMake Makefiles
@if "%2"=="generator_nmake" call "%VS80COMNTOOLS%vsvars32.bat" x86
@if "%2"=="generator_mingw" set generator_name=MinGW Makefiles
@set compiler=%3
@set configuration=%4
@set cmake_params=-G"%generator_name%" -DCMAKE_BUILD_TYPE:STRING=%configuration%

rem --- zlib ---
cd ..\dependencies\zlib
mkdir %build_dir%
cd %build_dir%
"..\..\cmake\bin\cmake.exe" %cmake_params% ../../cmake_scripts/zlib

%compiler%

rem --- libpng ---
cd ..\..\libpng
mkdir %build_dir%
cd %build_dir%
"..\..\cmake\bin\cmake.exe" %cmake_params% ../../cmake_scripts/libpng

%compiler%

rem --- libogg ---
cd ..\..\ogg
mkdir %build_dir%
cd %build_dir%
"..\..\cmake\bin\cmake.exe" %cmake_params% ../../cmake_scripts/ogg

%compiler%

rem --- expat ---

cd ..\..\expat
mkdir %build_dir%
cd %build_dir%
"..\..\cmake\bin\cmake.exe" %cmake_params% ../../cmake_scripts/expat

%compiler%

rem --- ois ---

cd ..\..\ois
mkdir %build_dir%
cd %build_dir%
"..\..\cmake\bin\cmake.exe" %cmake_params% ../../cmake_scripts/ois

%compiler%

rem --- OpenAL ---

cd ..\..\OpenAL
mkdir %build_dir%
cd %build_dir%
"..\..\cmake\bin\cmake.exe" %cmake_params% ../../cmake_scripts/OpenAL

%compiler%

rem --- pybind ---

cd ..\..\pybind
mkdir %build_dir%
cd %build_dir%
"..\..\cmake\bin\cmake.exe" %cmake_params% ../../cmake_scripts/pybind

%compiler%


rem --- libtheora ---
cd ..\..\theora
mkdir %build_dir%
cd %build_dir%
"..\..\cmake\bin\cmake.exe" %cmake_params% ../../cmake_scripts/theora

%compiler%

rem --- libvorbis ---
cd ..\..\vorbis
mkdir %build_dir%
cd %build_dir%
"..\..\cmake\bin\cmake.exe" %cmake_params% ../../cmake_scripts/vorbis

%compiler%

rem --- libpthreads ---
cd ..\..\pthreads
mkdir %build_dir%
cd %build_dir%
"..\..\cmake\bin\cmake.exe" %cmake_params% ../../cmake_scripts/pthreads

%compiler%

rem --- libpython ---
cd ..\..\python
mkdir %build_dir%
cd %build_dir%
"..\..\cmake\bin\cmake.exe" %cmake_params% ../../cmake_scripts/python

%compiler%

rem --- libjpeg ---
cd ..\..\libjpeg
@if "%1"=="build_msvc8" copy ..\cmake_scripts\libjpeg\jconfig.h.msvc8 jconfig.h
mkdir %build_dir%
cd %build_dir%
"..\..\cmake\bin\cmake.exe" %cmake_params% ../../cmake_scripts/libjpeg

%compiler%

rem --- libbox2d ---
cd ..\..\box2d
mkdir %build_dir%
cd %build_dir%
"..\..\cmake\bin\cmake.exe" %cmake_params% ../../cmake_scripts/box2d

%compiler%

cd ..\..\..\build