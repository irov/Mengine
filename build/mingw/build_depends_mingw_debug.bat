@echo off

@echo Starting dependencies build debug configuration...

@pushd ..
@call build_depends.bat "%CD%\..\CMake\Depends_MinGW" build_mingw_dependencies\Debug "MinGW Makefiles" Debug
@popd

@echo Done

@pause