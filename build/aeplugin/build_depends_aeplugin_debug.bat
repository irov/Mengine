@echo off

@echo Starting dependencies build debug configuration...

@pushd ..
@call vcvarsall_msvc15_x64
@call build_depends "%CD%\..\CMake\Depends_AEPlugin" build_msvc15_aeplugin_debug "Visual Studio 15 2017 Win64" Debug
@popd

@echo Done

@pause