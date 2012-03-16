set( MY_LIB_NAME RenderParticles)

ADD_FILTER(
src
	main.cpp
    RenderParticles.h
    RenderParticles.cpp
	RenderParticles.def
    ParticlesLogger.cpp
    ParticlesLogger.h 
    StringConversion.cpp
    StringConversion.h
    RenderParticlesScriptWrapper.h
    RenderParticlesScriptWrapper.cpp
    EmitterDescription.h
    EmitterDescription.cpp
    WinIncluder.h
    EmitterDescriptionManager.cpp
    EmitterDescriptionManager.h
    RenderParticlesSuites.h   
    RenderParticlesSuites.cpp
	RenderParticlesInterface.h
)


LINK_DIRECTORIES( ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE} ${APPLICATION_LIB_PATHS} )

add_definitions(-DUNICODE)

INCLUDE_DIRECTORIES( ${Mengine_SOURCE_DIR}/Utils 
					 ${THIRDPARTY_DIR}
					 ${THIRDPARTY_DIR}/pybind/include
					 ${THIRDPARTY_DIR}/python/include
                     ${THIRDPARTY_DIR}/boost
)

LINK_DIRECTORIES( ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
#ADD_EXECUTABLE( RenderParticles WIN32 ${SRC_FILES} )
#ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )


ADD_LIBRARY( ${MY_LIB_NAME} SHARED  ${SRC_FILES} )

SET( LIB_PREFIX )

foreach(dependency ${APPLICATION_DEPENDENCIES})
	#MESSAGE( ${LIB_PREFIX}${dependency}${MENGINE_LIB_SUFFIX} )
	TARGET_LINK_LIBRARIES( RenderParticles debug ${LIB_PREFIX}${dependency}${CMAKE_DEBUG_POSTFIX}${MENGINE_LIB_POSTFIX} )
	TARGET_LINK_LIBRARIES( RenderParticles optimized ${LIB_PREFIX}${dependency}${MENGINE_LIB_POSTFIX} )
endforeach(dependency ${APPLICATION_DEPENDENCIES})


TARGET_LINK_LIBRARIES( RenderParticles ${APPLICATION_LIBS} comctl32 )
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} WindowsLayer )
set_target_properties( RenderParticles PROPERTIES LINK_FLAGS /NODEFAULTLIB:"libci.lib" )


ADD_DEPENDENCIES( RenderParticles ${APPLICATION_DEPENDENCIES} )