@echo off

@echo Starting dependencies build debug configuration...

@pushd ..
@call build_depends.bat "%CD%\..\CMake\Depends_MinGW" build_mingw_dependencies_release\Release "MinGW Makefiles" Release
@popd

@echo Done

@pause