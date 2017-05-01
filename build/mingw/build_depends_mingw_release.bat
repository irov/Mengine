@echo off

@echo Starting dependencies build...

@pushd ..
@call cmake_configure "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\CMake\Depends_MinGW" "..\dependencies\build_mingw" "MinGW Makefiles" Release
@popd

@pushd ..\..\dependencies\build_mingw\Release
mingw32-make.exe
@popd


@echo Done
@pause

@echo off

@echo Starting dependencies build debug configuration...

@pushd ..
@call vcvarsall_msvc15
@call build_depends "%CD%\..\CMake\Depends_MinGW" build_mingw "Visual Studio 15 2017" Debug
@popd

@echo Done

@pause