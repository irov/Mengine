@call vcvarsall_msvc14

@echo Starting dependencies build debug configuration...

@pushd ..
rem @call build_depends "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\CMake\Depends_WIN32" "..\build_temp\build_msvc14_debug" "Visual Studio 14 2015" "" "-DCMAKE_CONFIGURATION_TYPES:STRING='Debug'" "-DCMAKE_BUILD_TYPE:STRING='Debug'"
@call build_depends "%CD%\..\CMake\Depends_WIN32" build_msvc14_debug "Visual Studio 14 2015" Debug
@popd

@echo Done

@pause