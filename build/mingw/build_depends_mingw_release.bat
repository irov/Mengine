@echo off

@echo Starting dependencies build debug configuration...

@pushd ..
@call vcvarsall_msvc15
@call build_depends "%CD%\..\CMake\Depends_MinGW" build_mingw_dependencies_release "MinGW Makefiles" Release
@popd

@echo Done

@pause