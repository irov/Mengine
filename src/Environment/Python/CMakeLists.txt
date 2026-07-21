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
    PythonCallbackProviderHelper.h
    PythonCallbackProviderHelper.cpp
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

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})

if(MENGINE_PYBIND_BACKEND STREQUAL "python")
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}python${MENGINE_LIB_SUFFIX})
    TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PUBLIC MENGINE_PYBIND_RUNTIME_PYTHON)
elseif(MENGINE_PYBIND_BACKEND STREQUAL "tinypy")
    TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PUBLIC MENGINE_PYBIND_RUNTIME_TINYPY)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}tinypy${MENGINE_LIB_SUFFIX})
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)