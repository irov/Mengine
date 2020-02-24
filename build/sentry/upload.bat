set SENTRY_ORG=wonderland-9p
set SENTRY_PROJECT=mengine
set SENTRY_AUTH_TOKEN=f3be54b85a14485eba0296e15cfb4e344c20fe5427dd4856b5149310f14a4a6a
set SENTRY_PDB_PATH="d:\Projects\landbase_code\Mengine\pdb\Mengine_WIN32\Visual Studio 16 2019\Debug"
sentry-cli.exe --log-level=debug upload-dif --org %SENTRY_ORG% --project %SENTRY_PROJECT% %SENTRY_AUTH_TOKEN% %SENTRY_PDB_PATH%