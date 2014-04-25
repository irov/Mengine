@echo off

@set toolsdir=%~d0%~p0

if NOT EXIST %toolsdir%\wget\wget.exe goto wget_not_found

%toolsdir%\wget\wget.exe --no-clobber http://downloads.sourceforge.net/project/boost/boost/1.54.0/boost_1_54_0.7z

%toolsdir%\wget\wget.exe --no-clobber http://stahlworks.com/dev/unzip.exe
%toolsdir%\wget\wget.exe --no-clobber http://downloads.sourceforge.net/sevenzip/7za465.zip

%toolsdir%\wget\wget.exe --no-clobber http://www.cmake.org/files/v2.8/cmake-2.8.10.2-win32-x86.zip
%toolsdir%\wget\wget.exe --no-clobber http://zlib.net/zlib128.zip
:: %toolsdir%\wget\wget.exe --no-clobber ftp://sourceware.org/pub/pthreads-win32/pthreads-w32-2-9-1-release.tar.gz
%toolsdir%\wget\wget.exe --no-clobber http://download.sourceforge.net/libpng/lpng162.zip
%toolsdir%\wget\wget.exe --no-clobber http://www.ijg.org/files/jpegsr9.zip

%toolsdir%\wget\wget.exe --no-clobber http://subversion.tigris.org/files/documents/15/47914/svn-win32-1.6.6.zip

                                     
%toolsdir%\wget\wget.exe --no-clobber http://downloads.xiph.org/releases/ogg/libogg-1.3.0.zip
%toolsdir%\wget\wget.exe --no-clobber http://downloads.xiph.org/releases/vorbis/libvorbis-1.3.3.zip

%toolsdir%\wget\wget.exe --no-clobber http://curl.haxx.se/download/libcurl-7.19.3-win32-ssl-msvc.zip

:: %toolsdir%\wget\wget.exe --no-clobber http://downloads.sourceforge.net/project/tinyxml/tinyxml/2.6.2/tinyxml_2_6_2.zip
:: %toolsdir%\wget\wget.exe --no-clobber http://pugixml.googlecode.com/files/pugixml-1.2.zip

:: %toolsdir%\wget\wget.exe --no-clobber http://kcat.strangesoft.net/openal-releases/openal-soft-1.13.tar.bz2
%toolsdir%\wget\wget.exe --no-clobber http://kcat.strangesoft.net/openal-releases/openal-soft-1.15.1.tar.bz2


%toolsdir%\wget\wget.exe --no-clobber http://python.org/ftp/python/2.7.6/Python-2.7.6.tgz
%toolsdir%\wget\wget.exe --no-clobber http://python.org/ftp/python/3.3.1/Python-3.3.1.tgz

:: %toolsdir%\wget\wget.exe --no-clobber http://download.icu-project.org/files/icu4c/49.1.2/icu4c-49_1_2-src.zip





:: extracting 7za.exe
unzip -o 7za465.zip 7za.exe

if EXIST boost rmdir /s /q boost
7za x -y boost_1_54_0.7z
move boost_1_54_0 boost

:: extracting thirdparties
if EXIST CMake rmdir /s /q CMake
7za x -y cmake-2.8.10.2-win32-x86.zip
move cmake-2.8.10.2-win32-x86 CMake

if EXIST zlib rmdir /s /q zlib
7za x -y zlib128.zip
move zlib-1.2.8 zlib

:: if EXIST pthreads rmdir /s /q pthreads
:: 7za x -y pthreads-w32-2-9-1-release.tar.gz
:: 7za x -y pthreads-w32-2-9-1-release.tar
:: move pthreads-w32-2-9-1-release pthreads

if EXIST libpng rmdir /s /q libpng
7za x -y lpng162.zip
move lpng162 libpng

if EXIST libjpeg rmdir /s /q libjpeg
7za x -y jpegsr9.zip
move jpeg-9 libjpeg

if EXIST Python275 rmdir /s /q Python275
7za x -y Python-2.7.5.tgz
7za x -y Python-2.7.5.tar

if EXIST Python331 rmdir /s /q Python331
7za x -y Python-3.3.1.tgz
7za x -y Python-3.3.1.tar
move Python-3.3.1 Python331

if EXIST ogg rmdir /s /q ogg
7za x -y libogg-1.3.0.zip
move libogg-1.3.0 ogg

if EXIST vorbis rmdir /s /q vorbis
7za x -y libvorbis-1.3.3.zip
move libvorbis-1.3.3 vorbis

if EXIST libcurl rmdir /s /q libcurl
7za x -y -olibcurl libcurl-7.19.3-win32-ssl-msvc.zip 

:: if EXIST icu rmdir /s /q icu
:: 7za x -y icu4c-49_1_2-src.zip

if EXIST OpenAL rmdir /s /q openalsoft
7za x -y openal-soft-1.15.1.tar.bz2
7za x -y openal-soft-1.15.1.tar
move openal-soft-1.15.1 openalsoft


if EXIST svn rmdir /s /q svn
7za x -y svn-win32-1.6.6.zip    
move svn-win32-1.6.6 svn

:: svn\bin\svn export --force http://svn.python.org/projects/python/tags/r264/ Python
:: svn\bin\svn export --force http://svn.xiph.org/tags/ogg/libogg-1.1.3 ogg
:: svn\bin\svn export --force http://svn.xiph.org/tags/vorbis/libvorbis-1.2.0 vorbis
:: svn\bin\svn export --force http://svn.xiph.org/tags/theora/theora-1.0beta3 theora
:: svn\bin\svn checkout http://box2d.googlecode.com/svn/trunk box2d
svn\bin\svn checkout http://pugixml.googlecode.com/svn/trunk pugixml
svn\bin\svn checkout https://svn.code.sf.net/p/pybind/code/trunk pybind
svn\bin\svn checkout https://svn.code.sf.net/p/metabuf/code/trunk metabuf
svn\bin\svn checkout https://svn.code.sf.net/p/stdex2/code/trunk stdex
svn\bin\svn checkout http://svn.code.sf.net/p/utfcpp/code/v2_0 utf8
svn\bin\svn checkout http://lz4.googlecode.com/svn/trunk lz4
svn\bin\svn checkout https://svn.code.sf.net/p/libgvf/code/ libgvf
svn\bin\svn checkout https://svn.code.sf.net/p/fastpathfinder/code fastpathfinder
:: svn\bin\svn checkout https://svn.code.sf.net/p/xxbind/code/trunk xxbind
::svn\bin\svn checkout https://svn.code.sf.net/p/atlasallocator/code/trunk AtlasAllocator
:: svn\bin\svn checkout http://simpleini.googlecode.com/svn/trunk simpleini
::svn\bin\svn export --force -r2465 svn://connect.creativelabs.com/OpenAL/trunk OpenAL

goto end

:wget_not_found
echo wget tool was not found. Please place it into directory near this .bat file
echo you can get tool from http://users.ugent.be/~bpuype/cgi-bin/fetch.pl?dl=wget/wget.exe
goto end

:end
pause
echo on