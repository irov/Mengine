set(MY_LIB_NAME AstralaxParticlePlugin)

ADD_FILTER(
src
	AstralaxParticleModule.cpp
    AstralaxParticleModule.h	
    AstralaxEmitter.cpp
	AstralaxEmitter.h
    AstralaxEmitter2.cpp
	AstralaxEmitter2.h
	AstralaxEmitterContainer.cpp
	AstralaxEmitterContainer.h
	AstralaxParticleSystem.cpp
	AstralaxParticleSystem.h
	ResourceAstralax.cpp
	ResourceAstralax.h
    LoaderResourceAstralax.cpp
    LoaderResourceAstralax.h
    AstralaxService.h
	AstralaxService.cpp
    
    ParticleResourceValidateVisitor.cpp
    ParticleResourceValidateVisitor.h
    
    UnknownParticleEmitterInterface.h

    AstralaxParticlePlugin.cpp
    AstralaxParticlePlugin.h
    AstralaxParticlePlugin.def    
)

if(MENGINE_PLUGIN_ASTRALAX_DLL)
    add_definitions(-DMENGINE_PLUGIN_DLL)
endif()

add_definitions(-DMAGIC_3D)

INCLUDE_DIRECTORIES( 
    ${THIRDPARTY_DIR}/pybind/include    
    ${THIRDPARTY_DIR}/metabuf/include
    ${THIRDPARTY_DIR}/libmath/include
    )

if(MENGINE_PLATFORM_IOS)
  INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/astralax-ios )
else()
  INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/astralax )
endif()

if(MENGINE_PLUGIN_ASTRALAX_DLL)
    ADD_MENGINE_SHARED()
else()
    ADD_MENGINE_LIBRARY()
endif()    

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Plugins
)

if(MENGINE_PLATFORM_WIN32)
    if(MENGINE_TARGET_MINGW)
        #TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/astralax/libmagic3d.a)		
        TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/astralax/libmagic3d.a)
		#TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/astralax/x64/magic3d.lib)
    else()
		TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/astralax/magic3d.lib)		
    endif()
endif()
    
if(MENGINE_PLATFORM_WIN64)
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/astralax/magic3d_x64.lib)
endif()

if(MENGINE_PLATFORM_MAC)
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/astralax/magic3d.dylib)
endif()

if(MENGINE_PLATFORM_IOS)
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/astralax-ios/bitcode/libimagic3d.a)
endif()

if(MENGINE_PLATFORM_ANDROID)
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/astralax-ios/bitcode/libimagic3d.a)
endif()

if(MENGINE_PLUGIN_ASTRALAX_DLL)    
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel)
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

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}metabuf${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}python${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}zlib${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})
