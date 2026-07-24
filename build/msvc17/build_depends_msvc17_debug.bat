@setlocal
@call %~dp0build_depends_msvc17.bat "CONFIGURATION=Debug" %*
@set "MENGINE_RESULT=%errorlevel%"

@if not defined MENGINE_NO_PAUSE pause
@endlocal & exit /b %MENGINE_RESULT%
