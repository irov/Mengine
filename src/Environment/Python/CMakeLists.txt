MENGINE_PROJECT(Python)

ADD_FILTER(
src
    PythonIncluder.h
    PythonScriptWrapper.h
    PythonEventReceiver.h
    PythonEventReceiver.cpp
    PythonAnimatableEventReceiver.h
    PythonDocument.h
    PythonDocument.cpp
    PythonForwardDeclaration.h
    PythonCallbackProvider.h
    PythonCallbackProvider.cpp
    PythonTraceback.h
    PythonTraceback.cpp
)

if(MENGINE_TARGET_APPLE)
    ADD_FILTER(
    apple
        ApplePythonProvider.h
        ApplePythonProvider.mm
    )
endif()

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)

ADD_MENGINE_LIBRARY(Environment)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind_python${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}python${MENGINE_LIB_SUFFIX})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)