set(MY_LIB_NAME GOAPPlugin)

ADD_FILTER(
src
    GOAPPlugin.h
    GOAPPlugin.cpp
    GOAPPlugin.def
    
    GOAPInterface.h
    
    GOAPService.h
    GOAPService.cpp
    
    EngineCook.h
    EngineCook.cpp
)

ADD_FILTER(
Tasks
    TaskNodeEnable.h
    TaskNodeEnable.cpp
    TaskNodeDisable.h
    TaskNodeDisable.cpp
    TaskNodeDestroy.h
    TaskNodeDestroy.cpp
    TaskDelay.cpp
    TaskDelay.h
    TaskLocalDelay.h
    TaskLocalDelay.cpp
    TaskGlobalDelay.h
    TaskGlobalDelay.cpp
    TaskPrint.cpp
    TaskPrint.h
    TaskPickerableMouseButton.cpp
    TaskPickerableMouseButton.h
    TaskPickerableMouseEnter.cpp
    TaskPickerableMouseEnter.h
    TaskPickerableMouseLeave.cpp
    TaskPickerableMouseLeave.h
    TaskGlobalMouseButton.cpp
    TaskGlobalMouseButton.h
    TaskGlobalMouseMove.cpp
    TaskGlobalMouseMove.h
    TaskGlobalMouseWheel.cpp
    TaskGlobalMouseWheel.h
    TaskGlobalKeyPress.h
    TaskGlobalKeyPress.cpp
    TaskTransformationRotateY.cpp
    TaskTransformationRotateY.h
    TaskTransformationRotateYTime.cpp
    TaskTransformationRotateYTime.h
    TaskTransformationScaleTime.cpp
    TaskTransformationScaleTime.h
    TaskTransformationTranslate.cpp
    TaskTransformationTranslate.h
    TaskTransformationTranslateTime.h
    TaskTransformationTranslateTime.cpp
    TaskColorableAlphaTime.h
    TaskColorableAlphaTime.cpp
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
    TaskAnimatableRewind.h
    TaskAnimatableRewind.cpp
    TaskNotify.h
    TaskEventable.h
    TaskEventable.cpp
)

INCLUDE_DIRECTORIES(${MENGINE_CMAKE_DEPENDENCIES_DIR}/GOAP/Config)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/GOAP/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_GOAP)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}GOAP${MENGINE_LIB_SUFFIX})

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel)