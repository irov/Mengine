@call %~dp0..\..\..\..\build\make_solution.bat "CONFIGURATION=Debug" "SOLUTION_NAME=solution_msvc15_dx11" "DEPENDENCIES_PROJECT=Depends_WIN32_DX11" "SOLUTION_BASE_DIR=%~dp0..\..\solutions" "SOURCE_DIRECTORY=%~dp0..\..\cmake\Win32_DX11" "GENERATOR=Visual Studio 15 2017" "BUILD_PUBLISH=OFF"

@exit /b %errorlevel%
