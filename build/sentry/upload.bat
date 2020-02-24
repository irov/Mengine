set SENTRY_ORG=wonderland-9p
set SENTRY_PROJECT=mengine
set SENTRY_AUTH_TOKEN=%1
set SENTRY_PDB_PATH="d:\Projects\landbase_code\Mengine\pdb\Mengine_WIN32\Visual Studio 16 2019\Debug"
sentry-cli.exe --log-level=debug upload-dif --org %SENTRY_ORG% --project %SENTRY_PROJECT% %SENTRY_AUTH_TOKEN% %SENTRY_PDB_PATH%