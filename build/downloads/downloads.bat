@echo off

@echo Starting download dependencies...

@mkdir %~dp0..\..\build_temp\downloads
@pushd %~dp0..\..\build_temp\downloads
@call CMake.exe "%CD%\..\..\CMake\Downloads"
@popd

@pushd %~dp0..\..\build_temp\downloads
@call CMake.exe --build .
@popd

@echo Done

@pause