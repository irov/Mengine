@echo off

@set toolsdir=%~d0%~p0

if NOT EXIST %toolsdir%\wget\wget.exe goto wget_not_found

%toolsdir%\wget\wget.exe --no-clobber http://stahlworks.com/dev/unzip.exe
%toolsdir%\wget\wget.exe --no-clobber http://downloads.sourceforge.net/sevenzip/7za465.zip

:: extracting 7za.exe
unzip -o 7za465.zip 7za.exe

:: call:getdepend http://downloads.sourceforge.net/project/boost/boost/1.58.0/boost_1_58_0.7z boost_1_58_0.7z boost_1_58_0 boost
call:getdepend http://www.cmake.org/files/v3.1/cmake-3.1.2-win32-x86.zip cmake-3.1.2-win32-x86.zip cmake-3.1.2-win32-x86 CMake
call:getdepend http://zlib.net/zlib128.zip zlib128.zip zlib-1.2.8 zlib
call:getdepend http://download.sourceforge.net/libpng/lpng1616.zip lpng1616.zip lpng1616 libpng
call:getdepend http://www.ijg.org/files/jpegsr9a.zip jpegsr9a.zip jpeg-9a libjpeg
call:getdepend http://zlib.net/zlib128.zip zlib128.zip zlib-1.2.8 zlib
call:getdepend http://downloads.xiph.org/releases/ogg/libogg-1.3.2.zip libogg-1.3.2.zip libogg-1.3.2 libogg
call:getdepend http://downloads.xiph.org/releases/vorbis/libvorbis-1.3.4.zip libvorbis-1.3.4.zip libvorbis-1.3.4 libvorbis
call:getdepend http://downloads.xiph.org/releases/theora/libtheora-1.1.1.zip libtheora-1.1.1.zip libtheora-1.1.1 libtheora
call:getdepend_tgz http://kcat.strangesoft.net/openal-releases/openal-soft-1.16.0.tar.bz2 openal-soft-1.16.0.tar.bz2 openal-soft-1.16.0.tar openal-soft-1.16.0 openal-soft

call:getdepend_tgz https://www.python.org/ftp/python/2.7.10/Python-2.7.10.tgz Python-2.7.10.tgz Python-2.7.10.tar Python-2.7.10 Python
call:getdepend_tgz https://www.python.org/ftp/python/3.4.3/Python-3.4.3.tgz Python-3.4.3.tgz Python-3.4.3.tar Python-3.4.3 Python3

call:getdepend http://www.astralax.ru/download/programm/magic_win_140704.zip magic_win_140704.zip win astralax

call:svndepend http://pugixml.googlecode.com/svn/trunk pugixml
call:svndepend https://svn.code.sf.net/p/pybind/code/trunk pybind
call:svndepend https://svn.code.sf.net/p/metabuf/code/trunk metabuf
call:svndepend https://svn.code.sf.net/p/stdex2/code/trunk stdex
call:svndepend http://svn.code.sf.net/p/utfcpp/code/v2_0 utf8
call:svndepend http://lz4.googlecode.com/svn/trunk lz4
call:svndepend https://svn.code.sf.net/p/fastpathfinder/code fastpathfinder

echo.&pause&goto:eof

:svndepend

@echo svn: %~1

svn checkout -q %~1 %~2

goto:eof

:getdepend
%toolsdir%\wget\wget.exe --no-clobber --no-check-certificate %~1

@echo extract: %~1

if EXIST %~4 rmdir /s /q %~4
7za x -y %~2 > nul
move %~3 %~4
goto:eof

:getdepend_tgz
%toolsdir%\wget\wget.exe --no-clobber --no-check-certificate %~1

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