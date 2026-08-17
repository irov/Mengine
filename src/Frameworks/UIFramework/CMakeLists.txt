MENGINE_PROJECT(UIFramework)

ADD_FILTER(
UIFrameworkPlugin
    UIFrameworkPlugin.cpp
    UIFrameworkPlugin.h
    UIFrameworkPlugin.def
)

ADD_FILTER(
UILayout2D
    UILayout2D.cpp
    UILayout2D.h
)

ADD_FILTER(
UIVirtualCollection
    UIVirtualCollection.cpp
    UIVirtualCollection.h
    UIVirtualList.cpp
    UIVirtualList.h
    UIVirtualGrid.cpp
    UIVirtualGrid.h
)

ADD_FILTER(
UIControls
    UIControlCallbackInterface.h
    UIProgressBar.cpp
    UIProgressBar.h
    UISlider.cpp
    UISlider.h
    UIRadioGroup.cpp
    UIRadioGroup.h
    UIPageView.cpp
    UIPageView.h
    UIFocusScope.cpp
    UIFocusScope.h
)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    Embedding
        UIFrameworkScriptEmbedding.cpp
        UIFrameworkScriptEmbedding.h
    )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

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

TARGET_LINK_LIBRARIES(${PROJECT_NAME} GOAPCook)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Engine)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()
