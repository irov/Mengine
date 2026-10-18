MENGINE_PROJECT(MengineWebViewRuntime)

ADD_FILTER(
MengineWebViewRuntime
    ../MengineRuntime.h
    MengineWebViewRuntime.cpp
)

ADD_MENGINE_LIBRARY(Runtime)

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    ProviderService
    ${MENGINE_SYSTEM_ALLOCATOR}
    EnumeratorService
    StringizeService
    DocumentService
    WebViewBootstrapper
)
