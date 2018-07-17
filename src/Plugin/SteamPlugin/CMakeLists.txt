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
        ${THIRDPARTY_DIR}/steam/public
        ${THIRDPARTY_DIR}/libmath/include
)

if( MENGINE_PLUGIN_STEAM_DLL )
    ADD_MENGINE_SHARED()
else()
    ADD_MENGINE_LIBRARY()
endif()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Plugins
)

IF(MENGINE_PLATFORM_WIN32)
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/steam/redistributable_bin/steam_api.lib)
ENDIF()

if( MENGINE_PLATFORM_MAC )
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/steam/redistributable_bin/osx32/libsteam_api.dylib)
endif()

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})

if( MENGINE_PLUGIN_ASTRALAX_DLL )
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel )
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
      Kernel
      )

    SET( APPLICATION_LIBS
      ${APPLICATION_LIBS}
      PARENT_SCOPE
      )
endif()
