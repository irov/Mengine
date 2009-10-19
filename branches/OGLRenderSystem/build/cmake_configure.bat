
@set cmake_tool=%1
@if "%cmake_tool%"=="" goto error

@set source_dir=%2
@if "%source_dir%"=="" goto error

@set build_dir=%3
@if "%build_dir%"=="" set build_dir="build"

@set generator=%4
@if %generator%=="" set generator="Unix Makefiles"

@set configuration=%5
@if "%configuration%"=="" set configuration="Release"

@if %generator%=="NMake Makefiles" call "%VS80COMNTOOLS%vsvars32.bat" x86


@mkdir %build_dir%_%configuration%
@pushd %build_dir%_%configuration%

@echo %cmake_tool% -G%generator% %source_dir%
%cmake_tool% -G%generator% %source_dir%

@popd

goto end

:error

@echo CMake tool not defined

:end
