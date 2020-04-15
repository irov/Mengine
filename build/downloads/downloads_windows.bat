@echo off

@echo Starting download dependencies...

@mkdir %~dp0..\..\solutions\downloads
@pushd %~dp0..\..\solutions\downloads
@call CMake.exe "%CD%\..\..\cmake\Downloads" -DMENGINE_DOWNLOADS_SILENT=ON
@popd

@pushd %~dp0..\..\solutions\downloads
@call CMake.exe --build . -j 4 -- /verbosity:minimal
@popd

@echo Done

@pause