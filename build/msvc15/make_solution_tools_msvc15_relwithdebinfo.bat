@echo off

@pushd ..
@call vcvarsall_msvc15.bat
@popd

@pushd ..
@call make_solution.bat "%CD%\..\CMake\Tools_Win32" solution_tools_msvc15_relwithdebinfo "Visual Studio 15 2017" RelWithDebInfo build_msvc15_relwithdebinfo
@popd

@echo on

@pause

