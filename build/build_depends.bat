
@if "%1"=="" goto error

@if "%2"=="generator_nmake" call "%VS80COMNTOOLS%vsvars32.bat" x86

cd ..\dependencies

:: zlib
@call ..\build\_build_depend_ zlib %4 %1 %3 %2
:: libpng
@call ..\build\_build_depend_ libpng %4 %1 %3 %2

:: ogg
@call ..\build\_build_depend_ ogg %4 %1 %3 %2

:: expat
@call ..\build\_build_depend_ expat %4 %1 %3 %2

:: ois
@call ..\build\_build_depend_ ois %4 %1 %3 %2

:: OpenAL
copy /Y cmake_scripts\OpenAL\win_patch\al.h OpenAL\Include\AL\al.h
copy /Y cmake_scripts\OpenAL\win_patch\alc.h OpenAL\Include\AL\alc.h
copy /Y cmake_scripts\OpenAL\win_patch\ALc.c OpenAL\OpenAL-Windows\Alc\ALc.c

@call ..\build\_build_depend_ OpenAL %4 %1 %3 %2


:: theora
@call ..\build\_build_depend_ theora %4 %1 %3 %2

:: vorbis
@call ..\build\_build_depend_ vorbis %4 %1 %3 %2

:: pthreads
@call ..\build\_build_depend_ pthreads %4 %1 %3 %2


:: python
@if "%1"=="build_mingw" copy /Y cmake_scripts\Python\Python\dynload_win.c.mingw Python\Python\dynload_win.c
@if "%1"=="build_mingw" copy /Y cmake_scripts\Python\pyconfig.h.mingw Python\Include\pyconfig.h
@if "%1"=="build_mingw" copy /Y cmake_scripts\Python\Modules\posixmodule.c.mingw Python\Modules\posixmodule.c
@if "%1"=="build_msvc8" copy /Y cmake_scripts\Python\Python\dynload_win.c.msvc8 Python\Python\dynload_win.c
@if "%1"=="build_msvc8" copy /Y Python\PC\pyconfig.h Python\Include\pyconfig.h
@if "%1"=="build_msvc8" copy /Y cmake_scripts\Python\Modules\posixmodule.c.msvc8 Python\Modules\posixmodule.c
copy /Y cmake_scripts\Python\Modules\zipimport.c Python\Modules\zipimport.c
copy /Y cmake_scripts\Python\Include\MengeStdioWrapper.h Python\Include\MengeStdioWrapper.h

@call ..\build\_build_depend_ python %4 %1 %3 %2


:: pybind
@call ..\build\_build_depend_ pybind %4 %1 %3 %2


:: libjpeg
@if "%1"=="build_msvc8" copy cmake_scripts\libjpeg\jconfig.h.msvc8 libjpeg\jconfig.h
@if "%1"=="build_mingw" copy cmake_scripts\libjpeg\jconfig.h.mingw libjpeg\jconfig.h
copy /Y cmake_scripts\libjpeg\jmorecfg.h libjpeg\jmorecfg.h

@call ..\build\_build_depend_ libjpeg %4 %1 %3 %2

:: box2d
@call ..\build\_build_depend_ box2d %4 %1 %3 %2

cd ..\build

goto end

:error
@echo invalid use

:end