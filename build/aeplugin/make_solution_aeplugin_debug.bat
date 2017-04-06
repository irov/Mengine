@echo off

@pushd ..
@call vcvarsall_msvc15_x64
@call make_solution "%CD%\..\CMake\AEPlugin" solution_msvc15_aeplugin_debug "Visual Studio 15 2017 Win64" Debug build_msvc15_aeplugin_debug
@popd

@echo on

@pause

