set( MY_LIB_NAME MengeVideoCodecPlugin )

ADD_FILTER(
src
	VideoCodecPlugin.h
	VideoCodecPlugin.cpp
    VideoCodecDecoderSystem.h
    VideoCodecDecoderSystem.cpp
	VideoDecoder.h
	VideoDecoder.cpp
    VideoDecoderFFMPEG.cpp
	VideoDecoderFFMPEG.h
)

INCLUDE_DIRECTORIES( ${Mengine_SOURCE_DIR}/Utils 					 
					 ${THIRDPARTY_DIR}/ffmpeg/include                     
)

IF(MSVC)
    INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/cmake_scripts/ffmpeg )
ENDIF(MSVC)

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

#SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS}/EXPORT:dllCreatePlugin")

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)

SET( APPLICATION_LIB_PATHS
 ${APPLICATION_LIB_PATHS}
 ${THIRDPARTY_DIR}/ffmpeg/lib
)

SET( APPLICATION_LIBS
    ${APPLICATION_LIBS}
    
     debug ${THIRDPARTY_DIR}/ffmpeg/lib/avcodec.lib
     optimized ${THIRDPARTY_DIR}/ffmpeg/lib/avcodec.lib
     
     debug ${THIRDPARTY_DIR}/ffmpeg/lib/avfilter.lib
     optimized ${THIRDPARTY_DIR}/ffmpeg/lib/avfilter.lib
          
     debug ${THIRDPARTY_DIR}/ffmpeg/lib/avformat.lib
     optimized ${THIRDPARTY_DIR}/ffmpeg/lib/avformat.lib
     
     debug ${THIRDPARTY_DIR}/ffmpeg/lib/avutil.lib
     optimized ${THIRDPARTY_DIR}/ffmpeg/lib/avutil.lib
     
     debug ${THIRDPARTY_DIR}/ffmpeg/lib/postproc.lib
     optimized ${THIRDPARTY_DIR}/ffmpeg/lib/postproc.lib
     
     debug ${THIRDPARTY_DIR}/ffmpeg/lib/swresample.lib
     optimized ${THIRDPARTY_DIR}/ffmpeg/lib/swresample.lib
     
     debug ${THIRDPARTY_DIR}/ffmpeg/lib/swscale.lib
     optimized ${THIRDPARTY_DIR}/ffmpeg/lib/swscale.lib
    )

SET( APPLICATION_LIB_PATHS
 ${APPLICATION_LIB_PATHS}
 PARENT_SCOPE
)    
    
SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
 PARENT_SCOPE
)

