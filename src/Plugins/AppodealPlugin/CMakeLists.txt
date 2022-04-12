MENGINE_PROJECT(AppodealPlugin)

ADD_FILTER(
src
    # SentryInterface.h

    # SentryPlugin.h
    # SentryPlugin.def
    # SentryPlugin.cpp
)


if(MENGINE_TARGET_APPLE)
    ADD_FILTER(
    apple
        AppodealImplementation.h
        AppodealImplementation.m
    
    )
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPODEAL)

if(MENGINE_TARGET_APPLE)
    ADD_MENGINE_COCOAPOD_FUNC("appodeal")
endif()