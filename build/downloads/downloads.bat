@echo off

@echo Starting download dependencies...

@mkdir ..\..\build_temp\downloads
@pushd ..\..\build_temp\downloads
@call CMake.exe "%CD%\..\..\CMake\Downloads"
@popd

@pushd ..\..\build_temp\downloads
@call CMake.exe --build .\
@popd

@echo Done

@pause