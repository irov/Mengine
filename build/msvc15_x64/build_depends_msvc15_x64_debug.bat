@echo off

@echo Starting dependencies build debug configuration...

@pushd ..
@call vcvarsall_msvc15_x64.bat
@popd

@pushd ..
@call build_depends.bat "%CD%\..\CMake\Depends_WIN64" build_msvc15_x64_debug "Visual Studio 15 2017 Win64" Debug
@popd

@echo Done

@pause