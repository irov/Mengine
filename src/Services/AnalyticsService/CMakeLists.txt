MENGINE_PROJECT(AnalyticsService)

ADD_FILTER(
src
    AnalyticsService.h
    AnalyticsService.cpp
    
    AnalyticsFactoryInterface.h
    AnalyticsFactory.h
    AnalyticsFactory.cpp

    AnalyticsContext.h
    AnalyticsContext.cpp
    
    AnalyticsEvent.h
    AnalyticsEvent.cpp
    AnalyticsEventBuilder.h
    AnalyticsEventBuilder.cpp
    
    AnalyticsEventConstParameterBoolean.h
    AnalyticsEventConstParameterBoolean.cpp
    AnalyticsEventConstParameterString.h
    AnalyticsEventConstParameterString.cpp
    AnalyticsEventConstParameterInteger.h
    AnalyticsEventConstParameterInteger.cpp
    AnalyticsEventConstParameterDouble.h
    AnalyticsEventConstParameterDouble.cpp
    
    AnalyticsEventGetterParameterBoolean.h
    AnalyticsEventGetterParameterBoolean.cpp
    AnalyticsEventGetterParameterString.h
    AnalyticsEventGetterParameterString.cpp
    AnalyticsEventGetterParameterInteger.h
    AnalyticsEventGetterParameterInteger.cpp
    AnalyticsEventGetterParameterDouble.h
    AnalyticsEventGetterParameterDouble.cpp
)

ADD_MENGINE_LIBRARY(Services)