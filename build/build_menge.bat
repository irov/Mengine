
@set build_dir=%1
@if "%2"=="generator_nmake" set generator_name=NMake Makefiles
@if "%2"=="generator_nmake" call "%VS80COMNTOOLS%vsvars32.bat" x86
@if "%2"=="generator_mingw" set generator_name=MinGW Makefiles
@set compiler=%3

cd ..\
mkdir %build_dir%
cd %build_dir%
"..\dependencies\cmake\bin\cmake.exe" -G"%generator_name%" -DMENGINE_LIB_DIR:STRING="%build_dir%" ../src
%compiler%