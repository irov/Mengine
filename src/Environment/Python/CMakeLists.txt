MENGINE_PROJECT(Python)

ADD_FILTER(
src
    PythonIncluder.h
    PythonScriptWrapper.h
    PythonEventReceiver.h
    PythonAnimatableEventReceiver.h
    PythonDocumentTraceback.h
    PythonForwardDeclaration.h
    PythonCallbackProvider.h
)

ADD_MENGINE_INTERFACE(Environment)