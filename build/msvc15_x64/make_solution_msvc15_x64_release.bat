@echo off

@pushd ..
@call vcvarsall_msvc15_x64.bat
@popd

@pushd ..
@call make_solution.bat "%CD%\..\CMake\Win64" solution_msvc15_x64_release "Visual Studio 15 2017 Win64" Release build_msvc15_x64_release
@popd

@echo on

@pause
