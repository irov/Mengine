@echo off

@pushd ..
@call cmake_configure "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\CMake\Depends_MinGW" "..\build_temp\build_mingw_dependencies_release" "MinGW Makefiles" Release
@popd

@pushd ..\..\build_temp\build_mingw_dependencies_release\Release
mingw32-make.exe
@popd


@echo Done
@pause