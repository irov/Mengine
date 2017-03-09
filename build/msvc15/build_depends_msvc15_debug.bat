@echo off

@call vcvarsall_msvc15

@echo Starting dependencies build debug configuration...

@pushd ..
@call build_depends "%CD%\..\CMake\Depends_WIN32" build_msvc15_debug "Visual Studio 15 2017" Debug
@popd

@echo Done

@pause