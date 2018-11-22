@echo off

@echo Starting dependencies build release configuration...

@pushd ..
@call vcvarsall_msvc15_x64.bat
@popd

@pushd ..
@call build_depends.bat "%CD%\..\CMake\Depends_WIN64" build_msvc15_x64_release "Visual Studio 15 2017 Win64" Release
@popd

@echo Done

@pause