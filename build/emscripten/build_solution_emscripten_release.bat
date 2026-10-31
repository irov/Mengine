@setlocal
@call %~dp0build_solution_emscripten.bat Release %*
@set "MENGINE_RESULT=%errorlevel%"

@if not defined MENGINE_NO_PAUSE pause
@endlocal & exit /b %MENGINE_RESULT%
