@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

setlocal
call :setESC

set "CONFIGURATION=%1"

@echo Starting build dependencies mingw64 %CONFIGURATION% configuration...

@pushd %~dp0..
@call build_depends.bat "SOLUTION_NAME=dependencies_mingw64" "SOURCE_DIRECTORY=%CD%\..\cmake\Depends_MinGW64" "GENERATOR=Ninja" "CONFIGURATION=%CONFIGURATION%"
@popd

if errorlevel 1 (
    @echo %ESC%[91m*****************************************%ESC%[0m
    @echo %ESC%[91m***************  Failure  ***************%ESC%[0m
    @echo %ESC%[91m*****************************************%ESC%[0m
) else (
    @echo %ESC%[92m=========================================%ESC%[0m
    @echo %ESC%[92m=============  Successful  ==============%ESC%[0m
    @echo %ESC%[92m=========================================%ESC%[0m
)

:end
@echo Done

@pause

:setESC
for /F "tokens=1,2 delims=#" %%a in ('"prompt #$H#$E# & echo on & for %%b in (1) do rem"') do (
  set ESC=%%b
  exit /B 0
)