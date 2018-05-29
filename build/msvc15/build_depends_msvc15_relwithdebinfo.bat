@echo off

@echo Starting dependencies build relwithdebinfo configuration...

@pushd ..
@call vcvarsall_msvc15.bat
@popd

@pushd ..
@call build_depends.bat "%CD%\..\CMake\Depends_WIN32" build_msvc15_relwithdebinfo "Visual Studio 15 2017" RelWithDebInfo
@popd

@echo Done

@pause