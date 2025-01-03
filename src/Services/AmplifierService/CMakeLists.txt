MENGINE_PROJECT(AmplifierService)

ADD_FILTER(
src
    AmplifierService.h
    AmplifierService.cpp
    
    AmplifierSoundListener.h
    AmplifierSoundListener.cpp
)

ADD_MENGINE_LIBRARY(Services)