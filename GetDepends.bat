@echo off

if NOT EXIST wget\wget.exe goto wget_not_found

@set toolsdir=%~d0%~p0\dependencies
@set wget=%~d0%~p0\wget\wget.exe

mkdir %toolsdir%

cd %toolsdir%

%wget% --no-clobber http://stahlworks.com/dev/unzip.exe
%wget% --no-clobber http://www.7-zip.org/a/7za920.zip

:: extracting 7za.exe
unzip -o 7za920.zip 7za.exe

call:getdepend http://downloads.sourceforge.net/project/boost/boost/1.62.0/boost_1_62_0.7z boost_1_62_0.7z boost_1_62_0 boost

call:getdepend https://cmake.org/files/v3.7/cmake-3.7.1-win64-x64.zip cmake-3.7.1-win64-x64.zip cmake-3.7.1-win64-x64 cmake
call:getdepend http://download.sourceforge.net/libpng/lpng1623.zip lpng1623.zip lpng1623 libpng
call:getdepend http://www.ijg.org/files/jpegsr9b.zip jpegsr9b.zip jpeg-9b libjpeg
call:getdepend_2 https://storage.googleapis.com/downloads.webmproject.org/releases/webp/libwebp-0.5.2.tar.gz libwebp-0.5.2.tar libwebp-0.5.2 libwebp-0.5.2 libwebp 
call:getdepend http://zlib.net/zlib129.zip zlib129.zip zlib-1.2.9 zlib
call:getdepend http://downloads.xiph.org/releases/ogg/libogg-1.3.2.zip libogg-1.3.2.zip libogg-1.3.2 libogg
call:getdepend http://downloads.xiph.org/releases/vorbis/libvorbis-1.3.5.zip libvorbis-1.3.5.zip libvorbis-1.3.5 libvorbis
call:getdepend http://downloads.xiph.org/releases/theora/libtheora-1.1.1.zip libtheora-1.1.1.zip libtheora-1.1.1 libtheora
call:getdepend_2 http://kcat.strangesoft.net/openal-releases/openal-soft-1.17.2.tar.bz2 openal-soft-1.17.2.tar.bz2 openal-soft-1.17.2.tar openal-soft-1.17.2 openal-soft

call:getdepend_2 https://www.python.org/ftp/python/2.7.13/Python-2.7.13.tgz Python-2.7.13.tgz Python-2.7.13.tar Python-2.7.13 Python
call:getdepend_2 https://www.python.org/ftp/python/3.5.1/Python-3.5.1.tgz Python-3.5.1.tgz Python-3.5.1.tar Python-3.5.1 Python3

call:getdepend http://www.astralax.ru/download/programm/dev/magic_win.zip magic_win.zip win astralax
call:getdepend https://www.libsdl.org/release/SDL2-2.0.5.zip SDL2-2.0.5.zip SDL2-2.0.5 SDL2
call:getdepend https://www.libsdl.org/projects/SDL_net/release/SDL2_net-2.0.1.zip SDL2_net-2.0.1.zip SDL2_net-2.0.1 SDL2_net
call:getdepend https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-2.0.1.zip SDL2_mixer-2.0.1.zip SDL2_mixer-2.0.1 SDL2_mixer

call:svndepend https://github.com/zeux/pugixml.git/trunk pugixml
call:svndepend https://github.com/irov/pybind.git/trunk pybind
call:svndepend https://github.com/irov/metabuf.git/trunk metabuf
call:svndepend https://github.com/irov/stdex.git/trunk stdex
call:svndepend https://github.com/irov/movie.git/trunk libmovie
call:svndepend http://svn.code.sf.net/p/utfcpp/code/v2_0 utf8
call:svndepend https://github.com/lz4/lz4.git/trunk lz4
call:svndepend https://github.com/irov/fastpathfinder.git/trunk fastpathfinder
call:svndependr https://github.com/EsotericSoftware/spine-runtimes.git/trunk/spine-c spine-c 1587
call:svndepend https://github.com/zzzzrrr/poly2tri/trunk poly2tri
call:svndepend svn://svn.code.sf.net/p/polyclipping/code/trunk/cpp polyclipping
call:svndepend https://github.com/erincatto/Box2D.git/trunk/Box2D Box2D

echo.&pause&goto:eof

:svndepend

@echo svn: %~1

svn checkout -q %~1 %toolsdir%\%~2

goto:eof

:svndependr

@echo svn: %~1

svn checkout -q -r %3 %~1 %toolsdir%\%~2

goto:eof

:getdepend
%wget% --no-clobber --no-check-certificate %~1

@echo extract: %~1

if EXIST %~4 rmdir /s /q %~4
7za x -y %~2 > nul
move %~3 %~4
goto:eof

:getdepend_2
%wget% --no-clobber --no-check-certificate %~1

@echo extract: %~1

if EXIST %~5 rmdir /s /q %~5
7za x -y %~2 > nul
7za x -y %~3 > nul
move %~4 %~5
goto:eof

goto end

:wget_not_found
echo wget tool was not found. Please place it into directory near this .bat file
echo you can get tool from http://users.ugent.be/~bpuype/cgi-bin/fetch.pl?dl=wget/wget.exe
goto end

:end
pause
echo on