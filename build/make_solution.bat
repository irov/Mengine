set "CMAKE_EXE=%CD%\..\dependencies\cmake\bin\cmake.exe"
set "SOLUTION_DIR=..\solutions\%2"

@call cmake_configure %CMAKE_EXE% %1 %SOLUTION_DIR% %3 "" "-DCMAKE_CONFIGURATION_TYPES:STRING='%4'" "-DCMAKE_BUILD_TYPE:STRING='%4'" "-DMENGINE_LIB_DIR:STRING='%5'"
