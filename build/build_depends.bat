set "BUILD_TEMP_DIR=..\build_temp\%2"

@call cmake_configure.bat CMake %1 %BUILD_TEMP_DIR% %3 "" "-DCMAKE_CONFIGURATION_TYPES:STRING='%4'" "-DCMAKE_BUILD_TYPE:STRING='%4'"

@pushd %BUILD_TEMP_DIR%
CMake --build .\ --config %4 -- %5
@popd
