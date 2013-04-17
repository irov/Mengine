set( MY_LIB_NAME MengeVideoCodecPlugin )

ADD_FILTER(
src
	VideoCodecPlugin.def
	VideoCodecPlugin.h
	VideoCodecPlugin.cpp
#    VideoCodecDecoderSystem.h
#    VideoCodecDecoderSystem.cpp
    VideoDecoderFFMPEG.cpp
	VideoDecoderFFMPEG.h
#    VideoDecoderOGGTheora.cpp
#    VideoDecoderOGGTheora.h
)

INCLUDE_DIRECTORIES( ${Mengine_SOURCE_DIR}/Utils 					 
					 ${THIRDPARTY_DIR}/ffmpeg/include                     
)

IF(MSVC)
    INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/cmake_scripts/ffmpeg )
ENDIF(MSVC)

#ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )
ADD_LIBRARY( ${MY_LIB_NAME} SHARED ${SRC_FILES} )

SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS}/EXPORT:dllCreatePlugin")

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Logger)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Core)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Factory)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Codec)

IF(WIN32)
    SET(
     VIDEO_CODEC_STATIC_LIBS
     ${THIRDPARTY_DIR}/ffmpeg/lib/avcodec.lib
     ${THIRDPARTY_DIR}/ffmpeg/lib/avfilter.lib
     ${THIRDPARTY_DIR}/ffmpeg/lib/avformat.lib
     ${THIRDPARTY_DIR}/ffmpeg/lib/avutil.lib
     ${THIRDPARTY_DIR}/ffmpeg/lib/swresample.lib
     ${THIRDPARTY_DIR}/ffmpeg/lib/swscale.lib
     )
ENDIF(WIN32)

IF(MINGW)
    SET(
     VIDEO_CODEC_STATIC_LIBS
     ${THIRDPARTY_DIR}/ffmpeg/lib/libavcodec.dll.a
     ${THIRDPARTY_DIR}/ffmpeg/lib/libavfilter.dll.a
     ${THIRDPARTY_DIR}/ffmpeg/lib/libavformat.dll.a
     ${THIRDPARTY_DIR}/ffmpeg/lib/libavutil.dll.a
     ${THIRDPARTY_DIR}/ffmpeg/lib/libswresample.dll.a
     ${THIRDPARTY_DIR}/ffmpeg/lib/libswscale.dll.a
     )
ENDIF(MINGW)
 
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${VIDEO_CODEC_STATIC_LIBS} )

