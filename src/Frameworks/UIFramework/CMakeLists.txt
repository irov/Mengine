set(MY_LIB_NAME UIFramework)

ADD_FILTER(
UIFrameworkPlugin
    UIFrameworkPlugin.cpp
    UIFrameworkPlugin.h
)

ADD_FILTER(
UIFramework
    UIFramework.cpp
    UIFramework.h
)

ADD_FILTER(
Button
	Button.cpp
	Button.h
    ButtonEventReceiverInterface.h
    TaskButtonClick.cpp
    TaskButtonClick.h
)

ADD_FILTER(
Checkbox
	Checkbox.cpp
	Checkbox.h
    CheckboxEventReceiverInterface.h
    TaskCheckboxChange.cpp
    TaskCheckboxChange.h
)


INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/GOAP/include)
INCLUDE_DIRECTORIES(${MENGINE_CMAKE_DEPENDENCIES_DIR}/GOAP/Config)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_UIFRAMEWORK Frameworks)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}GOAP${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})
