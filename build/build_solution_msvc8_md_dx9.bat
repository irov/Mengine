@echo off
cd ..\
mkdir vs8_solution_md_dx9
cd vs8_solution_md_dx9
"..\dependencies\cmake\bin\cmake.exe" -DCMAKE_CONFIGURATION_TYPES:STRING="Debug;Release;RelWithDebInfo" -G"Visual Studio 8 2005" -DMENGINE_LIB_DIR:STRING="build_msvc8" -DMENGE_USE_MD:STRING=TRUE -DMENGE_DX9:STRING=TRUE ../src
@pause
@echo on
