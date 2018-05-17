@echo off

@echo Starting dependencies build release configuration...

@pushd ..
@call vcvarsall_msvc15.bat
@popd

@pushd ..
@call build_depends.bat "%CD%\..\CMake\Depends_WIN32_SDL" build_msvc15_sdl_release "Visual Studio 15 2017" Release
@popd

@echo Done

@pause