@echo off

setlocal EnableExtensions EnableDelayedExpansion
call :setESC

set "MENGINE_RESULT=0"

@echo Starting download dependencies...

@mkdir %~dp0..\..\solutions\downloads
@pushd %~dp0..\..\solutions\downloads
@call CMake.exe "%CD%\..\..\cmake\Downloads" -DMENGINE_DOWNLOADS_SILENT=ON
set "MENGINE_RESULT=!errorlevel!"
@popd

if !MENGINE_RESULT! NEQ 0 (
    @echo %ESC%[91m*****************************************%ESC%[0m
    @echo %ESC%[91m***************  Failure  ***************%ESC%[0m
    @echo %ESC%[91m*****************************************%ESC%[0m
    goto end
) else (
    @echo %ESC%[92m=========================================%ESC%[0m
    @echo %ESC%[92m=============  Successful  ==============%ESC%[0m
    @echo %ESC%[92m=========================================%ESC%[0m
)

@pushd %~dp0..\..\solutions\downloads
@call CMake.exe --build . -j 4
set "MENGINE_RESULT=!errorlevel!"
@popd

if !MENGINE_RESULT! NEQ 0 (
    @echo %ESC%[91m*****************************************%ESC%[0m
    @echo %ESC%[91m***************  Failure  ***************%ESC%[0m
    @echo %ESC%[91m*****************************************%ESC%[0m
    goto end
) else (
    @echo %ESC%[92m=========================================%ESC%[0m
    @echo %ESC%[92m=============  Successful  ==============%ESC%[0m
    @echo %ESC%[92m=========================================%ESC%[0m
)

@echo Done

:end

@if not defined MENGINE_NO_PAUSE pause
@exit /b !MENGINE_RESULT!

:setESC
for /F "tokens=1,2 delims=#" %%a in ('"prompt #$H#$E# & echo on & for %%b in (1) do rem"') do (
  set ESC=%%b
  exit /B 0
)
