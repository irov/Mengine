@echo off
cd ..\
mkdir vs8_solution
cd vs8_solution
"..\dependencies\cmake\bin\cmake.exe" -DCMAKE_CONFIGURATION_TYPES:STRING="Debug;Release" -G"Visual Studio 8 2005" -DMENGINE_LIB_DIR:STRING="build_nmake" ../src
if NOT "%1"=="/nopause" pause
