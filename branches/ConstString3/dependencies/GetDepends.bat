@echo off

@set toolsdir=%~d0%~p0

if NOT EXIST %toolsdir%\wget\wget.exe goto wget_not_found

%toolsdir%\wget\wget.exe --no-clobber http://downloads.sourceforge.net/project/boost/boost/1.48.0/boost_1_48_0.zip
%toolsdir%\wget\wget.exe --no-clobber http://stahlworks.com/dev/unzip.exe
%toolsdir%\wget\wget.exe --no-clobber http://downloads.sourceforge.net/sevenzip/7za465.zip

%toolsdir%\wget\wget.exe --no-clobber http://www.cmake.org/files/v2.8/cmake-2.8.6-win32-x86.zip
%toolsdir%\wget\wget.exe --no-clobber http://zlib.net/zlib125.zip
%toolsdir%\wget\wget.exe --no-clobber ftp://sourceware.org/pub/pthreads-win32/pthreads-w32-2-8-0-release.tar.gz
%toolsdir%\wget\wget.exe --no-clobber http://download.sourceforge.net/libpng/lpng157.zip
%toolsdir%\wget\wget.exe --no-clobber http://www.ijg.org/files/jpegsr8c.zip

%toolsdir%\wget\wget.exe --no-clobber http://box2d.googlecode.com/files/Box2D_v2.2.1.zip
%toolsdir%\wget\wget.exe --no-clobber http://subversion.tigris.org/files/documents/15/47914/svn-win32-1.6.6.zip
%toolsdir%\wget\wget.exe --no-clobber http://python.org/ftp/python/2.7.2/Python-2.7.2.tgz
%toolsdir%\wget\wget.exe --no-clobber http://ffmpeg.zeranoe.com/builds/win32/dev/ffmpeg-git-b55dd10-win32-dev.7z

%toolsdir%\wget\wget.exe --no-clobber http://downloads.xiph.org/releases/ogg/libogg-1.3.0.zip
%toolsdir%\wget\wget.exe --no-clobber http://downloads.xiph.org/releases/vorbis/libvorbis-1.3.2.zip

:: %toolsdir%\wget\wget.exe --no-clobber http://kcat.strangesoft.net/openal-releases/openal-soft-1.13.tar.bz2


if EXIST boost rmdir /s /q boost
7za x -y boost_1_48_0.zip
move boost_1_48_0 boost


:: extracting 7za.exe
unzip -o 7za465.zip 7za.exe

:: extracting thirdparties
if EXIST CMake rmdir /s /q CMake
7za x -y cmake-2.8.6-win32-x86.zip
move cmake-2.8.6-win32-x86 CMake

if EXIST zlib rmdir /s /q zlib
7za x -y zlib125.zip
move zlib-1.2.5 zlib

if EXIST pthreads rmdir /s /q pthreads
7za x -y pthreads-w32-2-8-0-release.tar.gz
7za x -y pthreads-w32-2-8-0-release.tar
move pthreads-w32-2-8-0-release pthreads

if EXIST libPNG rmdir /s /q libPNG
7za x -y lpng157.zip
move lpng157 libPNG

if EXIST libJPEG rmdir /s /q libJPEG
7za x -y jpegsr8c.zip
move jpeg-8c libJPEG

if EXIST Box2D rmdir /s /q Box2D
7za x -y Box2D_v2.2.1.zip
move Box2D_v2.2.1\Box2D Box2D
rmdir Box2D_v2.2.1

if EXIST Python rmdir /s /q Python
7za x -y Python-2.7.2.tgz
7za x -y Python-2.7.2.tar
move Python-2.7.2 Python

if EXIST ffmpeg rmdir /s /q ffmpeg
7za x -y ffmpeg-git-b55dd10-win32-dev.7z
move ffmpeg-git-b55dd10-win32-dev ffmpeg

if EXIST ogg rmdir /s /q ogg
7za x -y libogg-1.3.0.zip
move libogg-1.3.0 ogg

if EXIST vorbis rmdir /s /q vorbis
7za x -y libvorbis-1.3.2.zip
move libvorbis-1.3.2 vorbis


:: if EXIST OpenAL rmdir /s /q OpenAL
:: 7za x -y openal-soft-1.13.tar.bz2
:: 7za x -y openal-soft-1.13.tar
:: move openal-soft-1.13 OpenAL


if EXIST svn rmdir /s /q svn
7za x -y svn-win32-1.6.6.zip    
move svn-win32-1.6.6 svn
:: svn\bin\svn export --force http://svn.python.org/projects/python/tags/r264/ Python
:: svn\bin\svn export --force http://svn.xiph.org/tags/ogg/libogg-1.1.3 ogg
:: svn\bin\svn export --force http://svn.xiph.org/tags/vorbis/libvorbis-1.2.0 vorbis
:: svn\bin\svn export --force http://svn.xiph.org/tags/theora/theora-1.0beta3 theora
svn\bin\svn checkout https://pybind.svn.sourceforge.net/svnroot/pybind pybind
::svn\bin\svn export --force -r2465 svn://connect.creativelabs.com/OpenAL/trunk OpenAL

goto end

:wget_not_found
echo wget tool was not found. Please place it into directory near this .bat file
echo you can get tool from http://users.ugent.be/~bpuype/cgi-bin/fetch.pl?dl=wget/wget.exe
goto end

:end
pause
echo on