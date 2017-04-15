@echo off

@echo Starting dependencies build release configuration...

@pushd ..
@call vcvarsall_msvc15
@call build_depends "%CD%\..\CMake\Depends_WIN32" build_msvc15_release "Visual Studio 15 2017" Release
@popd

@echo Done

@pause