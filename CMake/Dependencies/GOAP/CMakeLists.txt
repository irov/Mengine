CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(GOAP)

SET( SOURCE_DIR ${THIRDPARTY_DIR}/GOAP )

# OUR SOURCES
set( sources
${SOURCE_DIR}/src/Chain.cpp
${SOURCE_DIR}/src/Source.cpp
${SOURCE_DIR}/src/Task.cpp
${SOURCE_DIR}/src/TaskCallback.cpp
${SOURCE_DIR}/src/TaskDeadLock.cpp
${SOURCE_DIR}/src/TaskDummy.cpp
${SOURCE_DIR}/src/TaskFork.cpp
${SOURCE_DIR}/src/TaskFunction.cpp
${SOURCE_DIR}/src/TaskGuard.cpp
${SOURCE_DIR}/src/TaskIf.cpp
${SOURCE_DIR}/src/TaskParallelNeck.cpp
${SOURCE_DIR}/src/TaskRaceNeck.cpp
${SOURCE_DIR}/src/TaskRepeat.cpp
${SOURCE_DIR}/src/TaskScope.cpp
${SOURCE_DIR}/src/TaskSwitch.cpp
${SOURCE_DIR}/src/TranscriptorBase.cpp
${SOURCE_DIR}/src/TranscriptorParallel.cpp
${SOURCE_DIR}/src/TranscriptorRace.cpp
)

include_directories(${SOURCE_DIR}/include)

add_library( ${PROJECT_NAME} STATIC ${sources} )

