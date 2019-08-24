SET(MY_LIB_NAME Tasks)

ADD_FILTER(
Tasks
    TaskDelay.cpp
    TaskDelay.h
    TaskPrint.cpp
    TaskPrint.h
    TaskPickerableMouseButton.cpp
    TaskPickerableMouseButton.h
    TaskGlobalMouseButton.cpp
    TaskGlobalMouseButton.h
    TaskGlobalMouseMove.cpp
    TaskGlobalMouseMove.h
    TaskGlobalMouseWheel.cpp
    TaskGlobalMouseWheel.h
    TaskGlobalMouseWheel.h
    TaskGlobalMouseWheel.cpp
	TaskGlobalKeyPress.h
	TaskGlobalKeyPress.cpp
    TaskTransformationRotateY.cpp
    TaskTransformationRotateY.h
    TaskTransformationRotateYTime.cpp
    TaskTransformationRotateYTime.h
    TaskTransformationTranslate.cpp
    TaskTransformationTranslate.h
    TaskTransformationTranslateTime.h
    TaskTransformationTranslateTime.cpp
    TaskAnimatablePlay.cpp
    TaskAnimatablePlay.h
    TaskAnimatablePlayWait.cpp
    TaskAnimatablePlayWait.h
    TaskAnimatableStop.cpp
    TaskAnimatableStop.h
    TaskAnimatablePause.cpp
    TaskAnimatablePause.h
    TaskAnimatableResume.cpp
    TaskAnimatableResume.h
	TaskAnimatablePlayReceiver.h
	TaskAnimatablePlayReceiver.cpp
    TaskHttpBase.h
    TaskHttpBase.cpp
    TaskHttpGet.h
    TaskHttpGet.cpp
    TaskHttpPost.h
    TaskHttpPost.cpp
    TaskHttpHeaderData.h
    TaskHttpHeaderData.cpp
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libmath/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/GOAP/include)
INCLUDE_DIRECTORIES(${MENGINE_CMAKE_DEPENDENCIES_DIR}/GOAP/Config)

ADD_MENGINE_LIBRARY()

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}GOAP${MENGINE_LIB_SUFFIX})
