SET( MY_LIB_NAME Codec )

ADD_FILTER(
src
    PickEncoder.cpp
    PickEncoder.h
    PickDecoder.cpp
    PickDecoder.h
    VideoDecoder.h
    VideoDecoder.cpp
    SoundDecoder.h
    SoundDecoder.cpp
    ImageDecoder.h
    ImageDecoder.cpp
    ImageEncoder.cpp
    ImageEncoder.h    
    
    DecoderFactory.h
    EncoderFactory.h    
    ConverterFactory.h
)

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

INCLUDE_DIRECTORIES( ${Mengine_SOURCE_DIR}/Utils ) 

SET( MENGE_DEPENDENCIES
 ${MENGE_DEPENDENCIES}
 ${MY_LIB_NAME}
)
 
SET( MENGE_DEPENDENCIES
 ${MENGE_DEPENDENCIES}
 PARENT_SCOPE
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)


SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)
