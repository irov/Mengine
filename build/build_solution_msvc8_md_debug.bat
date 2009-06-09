@echo off
cd ..\
mkdir vs8_solution_md
cd vs8_solution_md
"..\dependencies\cmake\bin\cmake.exe" -DCMAKE_CONFIGURATION_TYPES:STRING="Debug;Release;RelWithDebInfo" -G"Visual Studio 8 2005" -DMENGINE_LIB_DIR:STRING="build_msvc8" -DMENGE_USE_MD:STRING=TRUE ../src
@pause
@echo on
