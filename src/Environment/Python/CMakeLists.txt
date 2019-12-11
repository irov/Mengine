set(MY_LIB_NAME Python)

ADD_FILTER(
src     
    PythonScriptWrapper.h
    PythonEventReceiver.h
    PythonAnimatableEventReceiver.h    
)

ADD_FILTER(
dummy
    dummy.cpp
)

ADD_MENGINE_LIBRARY(Environment)