MENGINE_PROJECT(WebViewBootstrapper)

ADD_FILTER(
WebViewBootstrapper
    WebViewBootstrapper.h
    WebViewBootstrapper.cpp
)

ADD_MENGINE_LIBRARY(Bootstrapper)

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    Kernel
    LifecycleService
    NotificationService
    VocabularyService
    OptionsService
    FactoryService
    PrototypeService
    MemoryService
    StatisticService
    LoggerService
    ConfigService
    ThreadService
    FileService
    ResourceService
    TimerService
    TimepipeService
    UpdateService
    PluginService
    WebViewBridgeService
)

FOREACH(System MENGINE_SYSTEM_THREAD MENGINE_SYSTEM_TIME MENGINE_SYSTEM_DATETIME MENGINE_SYSTEM_FILE MENGINE_SYSTEM_CRYPTOGRAPHY MENGINE_SYSTEM_HTTP MENGINE_SYSTEM_SOCKET)
    IF(${System})
        TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${${System}})
    ENDIF()
ENDFOREACH()

IF(MENGINE_SYSTEM_HTTP)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} HttpService)
ENDIF()

IF(MENGINE_SYSTEM_SOCKET)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} SocketService)
ENDIF()

IF(MENGINE_TARGET_WINDOWS)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Windows)
ENDIF()
