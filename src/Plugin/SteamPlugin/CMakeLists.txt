set( MY_LIB_NAME SteamPlugin )

ADD_FILTER(
src
	SteamPlugin.h
	SteamPlugin.cpp   
    
    ModuleSteam.cpp
    ModuleSteam.h
)

if( MENGINE_PLUGIN_STEAM_DLL )
   add_definitions(-DMENGINE_PLUGIN_DLL)
endif()

INCLUDE_DIRECTORIES( 
        ${Mengine_SOURCE_DIR}/Utils 
        ${THIRDPARTY_DIR}/steam/include
        ${THIRDPARTY_DIR}/libmath/include
)

if( MENGINE_PLUGIN_STEAM_DLL )
    ADD_LIBRARY( ${MY_LIB_NAME} SHARED ${SRC_FILES} )
else()
    ADD_LIBRARY( ${MY_LIB_NAME} STATIC ${SRC_FILES} )
endif()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Plugins
)

IF(MENGINE_PLATFORM_WIN32)
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/steam/lib/steam_api.lib)
ENDIF()

if( MENGINE_PLATFORM_MAC )
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/steam/lib/libsteam_api.dylib)
endif()

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})

if( MENGINE_PLUGIN_ASTRALAX_DLL )
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Logger )
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Core )
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Factory )   
else()
    SET( APPLICATION_DEPENDENCIES
      ${APPLICATION_DEPENDENCIES}
      ${MY_LIB_NAME}
      )

    SET( APPLICATION_DEPENDENCIES
      ${APPLICATION_DEPENDENCIES}
      PARENT_SCOPE
      )  

    SET( APPLICATION_LIBS
      ${APPLICATION_LIBS}
      Logger
      Core
      Factory
      )

    SET( APPLICATION_LIBS
      ${APPLICATION_LIBS}
      PARENT_SCOPE
      )
endif()
