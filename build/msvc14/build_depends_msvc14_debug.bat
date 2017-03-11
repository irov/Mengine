@echo off

@call vcvarsall_msvc14

@echo Starting dependencies build debug configuration...

@pushd ..
@call build_depends "%CD%\..\CMake\Depends_WIN32" build_msvc14_debug "Visual Studio 14 2015" Debug
@popd

@echo Done

@echo on

@pause