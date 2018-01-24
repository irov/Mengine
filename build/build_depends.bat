set "BUILD_TEMP_DIR=..\build_temp\%2"
set "CMAKE_EXE=%CD%\..\dependencies\cmake\bin\cmake.exe"

@call cmake_configure %CMAKE_EXE% %1 %BUILD_TEMP_DIR% %3 "" "-DCMAKE_CONFIGURATION_TYPES:STRING='%4'" "-DCMAKE_BUILD_TYPE:STRING='%4'"

@pushd %BUILD_TEMP_DIR%
%CMAKE_EXE% --build .\ --config %4 
@popd
