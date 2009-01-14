
@set build_dir=%1
@if "%2"=="generator_nmake" set generator_name=NMake Makefiles
@if "%2"=="generator_nmake" call "%VS80COMNTOOLS%vsvars32.bat" x86
@if "%2"=="generator_mingw" set generator_name=MinGW Makefiles
@set compiler=%3
@set configuration=%4
@set cmake_params=-G"%generator_name%" -DCMAKE_BUILD_TYPE:STRING=%configuration% ../

rem --- zlib ---
cd ..\dependencies\zlib
mkdir %build_dir%
cd %build_dir%
"..\..\cmake\bin\cmake.exe" %cmake_params%

%compiler%

rem --- libpng ---
cd ..\..\libpng
mkdir %build_dir%
cd %build_dir%
"..\..\cmake\bin\cmake.exe" %cmake_params%

%compiler%

rem --- libogg ---
cd ..\..\ogg
mkdir %build_dir%
cd %build_dir%
"..\..\cmake\bin\cmake.exe" %cmake_params%

%compiler%

rem --- expat ---

cd ..\..\expat
mkdir %build_dir%
cd %build_dir%
"..\..\cmake\bin\cmake.exe" %cmake_params%

%compiler%

rem --- ois ---

cd ..\..\ois
mkdir %build_dir%
cd %build_dir%
"..\..\cmake\bin\cmake.exe" %cmake_params%

%compiler%

rem --- OpenAL ---

cd ..\..\OpenAL
mkdir %build_dir%
cd %build_dir%
"..\..\cmake\bin\cmake.exe" %cmake_params%

%compiler%

rem --- pybind ---

cd ..\..\pybind
mkdir %build_dir%
cd %build_dir%
"..\..\cmake\bin\cmake.exe" %cmake_params%

%compiler%


rem --- libtheora ---
cd ..\..\theora
mkdir %build_dir%
cd %build_dir%
"..\..\cmake\bin\cmake.exe" %cmake_params%

%compiler%

rem --- libvorbis ---
cd ..\..\vorbis
mkdir %build_dir%
cd %build_dir%
"..\..\cmake\bin\cmake.exe" %cmake_params%

%compiler%

rem --- libpthreads ---
cd ..\..\pthreads
mkdir %build_dir%
cd %build_dir%
"..\..\cmake\bin\cmake.exe" %cmake_params%

%compiler%

rem --- libpython ---
cd ..\..\python
mkdir %build_dir%
cd %build_dir%
"..\..\cmake\bin\cmake.exe" %cmake_params%/

%compiler%

rem --- libjpeg ---
cd ..\..\libjpeg
@if "%2"=="generator_nmake" copy jconfig.h.cmakenmake jconfig.h
mkdir %build_dir%
cd %build_dir%
"..\..\cmake\bin\cmake.exe" %cmake_params%

%compiler%

rem --- libbox2d ---
cd ..\..\box2d
mkdir %build_dir%
cd %build_dir%
"..\..\cmake\bin\cmake.exe" %cmake_params%

%compiler%
