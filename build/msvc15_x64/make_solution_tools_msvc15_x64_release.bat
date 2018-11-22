@echo off

@pushd ..
@call vcvarsall_msvc15.bat
@popd

@pushd ..
@call make_solution.bat "%CD%\..\CMake\Tools_Win32" solution_tools_msvc15_release "Visual Studio 15 2017" Release build_msvc15_release
@popd

@echo on

@pause

