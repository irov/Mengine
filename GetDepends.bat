@echo off

if NOT EXIST "wget\wget.exe" (
	goto wget_not_found
)

@set toolsdir=%~d0%~p0\dependencies
@set wget=%~d0%~p0\wget\wget.exe

IF NOT EXIST "%toolsdir%" (
	mkdir %toolsdir%
)

pushd %toolsdir%

IF NOT EXIST "7za.exe" (
	%wget% --no-clobber --no-check-certificate http://stahlworks.com/dev/unzip.exe
	%wget% --no-clobber --no-check-certificate https://www.7-zip.org/a/7za920.zip
	unzip -o 7za920.zip 7za.exe
)

getdepend http://www.ijg.org/files/jpegsr9c.zip jpegsr9c.zip jpeg-9c libjpeg
getdepend http://www.astralax.ru/download/programm/dev/magic_win.zip magic_win.zip win astralax

gitdepend https://github.com/boostorg/boost.git boost
gitdepend https://github.com/madler/zlib.git zlib
gitdepend https://github.com/xiph/ogg.git libogg
gitdepend https://github.com/xiph/vorbis.git libvorbis
gitdepend https://github.com/aseprite/freetype2.git freetype
gitdepend https://github.com/webmproject/libwebp.git libwebp
gitdepend https://github.com/zeux/pugixml.git pugixml
gitdepend https://github.com/irov/pybind.git pybind
gitdepend https://github.com/irov/metabuf.git metabuf
gitdepend https://github.com/irov/stdex.git stdex
gitdepend https://github.com/irov/movie.git libmovie
gitdepend https://github.com/nemtrif/utfcpp.git utf8
gitdepend https://github.com/lz4/lz4.git lz4
gitdepend https://github.com/irov/fastpathfinder.git fastpathfinder
gitdepend https://github.com/zzzzrrr/poly2tri poly2tri
gitdepend https://github.com/erincatto/Box2D.git Box2D
gitdepend https://github.com/irov/GOAP.git GOAP
gitdepend https://github.com/curl/curl.git libcurl
gitdepend https://github.com/irov/tinyini.git tinyini
gitdepend https://github.com/spurious/SDL-mirror.git SDL2
gitdepend https://github.com/frankinshtein/font-effects-lib.git libfe
gitdepend https://github.com/irov/math.git libmath
gitdepend https://github.com/akheron/jansson.git jansson

gitdependr https://github.com/glennrp/libpng.git libpng libpng17
gitdependr https://github.com/xiph/theora.git libtheora theora-1.1
gitdependr https://github.com/kcat/openal-soft.git openal-soft v1.18
gitdependr https://github.com/python/cpython.git Python 2.7
gitdependr https://github.com/python/cpython.git Python3 v3.6.5

svndepend svn://svn.code.sf.net/p/polyclipping/code/trunk/cpp polyclipping

goto end

:svndepend

@echo svn checkout: %~1

IF NOT EXIST "%toolsdir%\%~2" (
	svn checkout -q %~1 "%toolsdir%\%~2"
)

goto:eof

:svndependr

@echo svn checkout: %~1

IF NOT EXIST "%toolsdir%\%~2" (
	svn checkout -q -r %3 %~1 "%toolsdir%\%~2"
)

goto:eof

:gitdepend

@echo git clone: %~1

IF NOT EXIST "%toolsdir%\%~2" (
	git clone --recurse-submodules %~1 "%toolsdir%\%~2"
)

goto:eof

:gitdependr

@echo git clone: %~1

IF NOT EXIST "%toolsdir%\%~2" (
	git clone --recurse-submodules -b %~3 %~1 "%toolsdir%\%~2"
)

goto:eof

:getdepend
@echo download: %~1

IF NOT EXIST "%toolsdir%\%~4" (
	%wget% --no-clobber --no-check-certificate %~1
	7za x -y %~2 > nul
	move %~3 %~4
)

goto:eof

:getdepend_2
@echo download: %~1

IF NOT EXIST "%toolsdir%\%~4" (
	%wget% --no-clobber --no-check-certificate %~1

	7za x -y %~2 > nul
	7za x -y %~3 > nul
	move %~4 %~5
)

goto:eof

goto end

:wget_not_found
echo wget tool was not found. Please place it into directory near this .bat file
echo you can get tool from http://users.ugent.be/~bpuype/cgi-bin/fetch.pl?dl=wget/wget.exe
goto end

:end
popd

pause
echo on