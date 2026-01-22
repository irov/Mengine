MENGINE_PROJECT(UIFramework)

ADD_FILTER(
UIFrameworkPlugin
    UIFrameworkPlugin.cpp
    UIFrameworkPlugin.h
    UIFrameworkPlugin.def
)

ADD_FILTER(
Button
    Button.cpp
    Button.h
    ButtonEventReceiverInterface.h
    TaskButtonClick.cpp
    TaskButtonClick.h
    RouterButtonEventReceiver.h
    RouterButtonEventReceiver.cpp
    TaskRouterButtonClick.h
    TaskRouterButtonClick.cpp
)

ADD_FILTER(
Checkbox
    Checkbox.cpp
    Checkbox.h
    CheckboxEventReceiverInterface.h
    TaskCheckboxChange.cpp
    TaskCheckboxChange.h
    TaskCheckboxChangeAny.cpp
    TaskCheckboxChangeAny.h
)

INCLUDE_DIRECTORIES(${MENGINE_CMAKE_DEPENDENCIES_DIR}/GOAP/Config)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/GOAP/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_UIFRAMEWORK Frameworks)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}GOAP${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} GOAPCook)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)