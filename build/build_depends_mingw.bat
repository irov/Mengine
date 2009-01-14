@echo off

rem --- zlib ---
cd ..\dependencies\zlib
mkdir mingw_build
cd mingw_build
"..\..\cmake\bin\cmake.exe" -G"MinGW Makefiles" ../

C:\MinGW\bin\mingw32-make --directory D:\Development\Menge\Dependencies\zlib\mingw_build --makefile=Makefile

rem --- libpng ---
cd ..\..\libpng
mkdir mingw_build
cd mingw_build
"..\..\cmake\bin\cmake.exe" -G"MinGW Makefiles" ../

C:\MinGW\bin\mingw32-make --directory D:\Development\Menge\Dependencies\libpng\mingw_build --makefile=Makefile

rem --- libogg ---
cd ..\..\ogg
mkdir mingw_build
cd mingw_build
"..\..\cmake\bin\cmake.exe" -G"MinGW Makefiles" ../

C:\MinGW\bin\mingw32-make --directory D:\Development\Menge\Dependencies\ogg\mingw_build --makefile=Makefile

rem --- expat ---

cd ..\..\expat
mkdir mingw_build
cd mingw_build
"..\..\cmake\bin\cmake.exe" -G"MinGW Makefiles" ../

C:\MinGW\bin\mingw32-make --directory D:\Development\Menge\Dependencies\expat\mingw_build --makefile=Makefile

rem --- ois ---

cd ..\..\ois
mkdir mingw_build
cd mingw_build
"..\..\cmake\bin\cmake.exe" -G"MinGW Makefiles" ../

C:\MinGW\bin\mingw32-make --directory D:\Development\Menge\Dependencies\ois\mingw_build --makefile=Makefile

rem --- OpenAL ---

cd ..\..\OpenAL
mkdir mingw_build
cd mingw_build
"..\..\cmake\bin\cmake.exe" -G"MinGW Makefiles" ../

C:\MinGW\bin\mingw32-make --directory D:\Development\Menge\Dependencies\OpenAL\mingw_build --makefile=Makefile

rem --- pybind ---

cd ..\..\pybind
mkdir mingw_build
cd mingw_build
"..\..\cmake\bin\cmake.exe" -G"MinGW Makefiles" ../

C:\MinGW\bin\mingw32-make --directory D:\Development\Menge\Dependencies\pybind\mingw_build --makefile=Makefile


rem --- libtheora ---
cd ..\..\theora
mkdir mingw_build
cd mingw_build
"..\..\cmake\bin\cmake.exe" -G"MinGW Makefiles" ../

C:\MinGW\bin\mingw32-make --directory D:\Development\Menge\Dependencies\theora\mingw_build --makefile=Makefile

rem --- libvorbis ---
cd ..\..\vorbis
mkdir mingw_build
cd mingw_build
"..\..\cmake\bin\cmake.exe" -G"MinGW Makefiles" ../

C:\MinGW\bin\mingw32-make --directory D:\Development\Menge\Dependencies\vorbis\mingw_build --makefile=Makefile

rem --- libpthreads ---
cd ..\..\pthreads
mkdir mingw_build
cd mingw_build
"..\..\cmake\bin\cmake.exe" -G"MinGW Makefiles" ../

C:\MinGW\bin\mingw32-make --directory D:\Development\Menge\Dependencies\pthreads\mingw_build --makefile=Makefile

rem --- libpython ---
cd ..\..\python
mkdir mingw_build
cd mingw_build
"..\..\cmake\bin\cmake.exe" -G"MinGW Makefiles" ../

C:\MinGW\bin\mingw32-make --directory D:\Development\Menge\Dependencies\python\mingw_build --makefile=Makefile
if NOT "%1"=="/nopause" pause

