@call %~dp0..\..\..\..\build\make_solution.bat "CONFIGURATION=Debug" "SOLUTION_NAME=solution_msvc17" "DEPENDENCIES_PROJECT=Depends_WIN32" "SOLUTION_BASE_DIR=%~dp0..\..\solutions" "SOURCE_DIRECTORY=%~dp0..\..\cmake\Win32" "GENERATOR=Visual Studio 17 2022" "ARCHITECTURE=Win32" "BUILD_PUBLISH=OFF"

@exit /b %errorlevel%
