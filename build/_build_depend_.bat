
@set libname=%1
@set configuration=%2
@set configuration_dir=%configuration%
@set build_dir=%3
@set compiler=%4
@set generator_name=%5

@if "%generator_name%"=="generator_nmake" set generator_name=NMake Makefiles
@if "%generator_name%"=="generator_mingw" set generator_name=MinGW Makefiles


@if "%configuration%"=="Debug_md" set configuration_dir=%configuration%
@if "%configuration%"=="Debug_md" set cmake_params=-DMENGE_USE_MD:STRING=TRUE
@if "%configuration%"=="Debug_md" set configuration=Debug


@if "%configuration%"=="Release_md" set configuration_dir=%configuration%
@if "%configuration%"=="Release_md" set cmake_params=-DMENGE_USE_MD:STRING=TRUE
@if "%configuration%"=="Release_md" set configuration=Release

@set cmake_params=-G"%generator_name%" -DCMAKE_BUILD_TYPE:STRING=%configuration% %cmake_params%

:: current dir must be \dependencies

cd %libname%
mkdir %build_dir%
mkdir %build_dir%\%configuration_dir%
cd %build_dir%\%configuration_dir%
..\..\..\cmake\bin\cmake.exe %cmake_params% ../../../cmake_scripts/%libname%
%compiler%

cd ..\..\..\