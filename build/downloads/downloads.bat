@echo off

@echo Starting download dependencies...

@mkdir %~dp0..\..\build_temp\downloads
@pushd %~dp0..\..\build_temp\downloads
@call CMake.exe "%CD%\..\..\CMake\Downloads" -DMENGINE_DOWNLOADS_SILENT=ON
@popd

@pushd %~dp0..\..\build_temp\downloads
@call CMake.exe --build . -- /verbosity:minimal
@popd

@echo Done

@pause