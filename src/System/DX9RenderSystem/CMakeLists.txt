set( MY_LIB_NAME DX9RenderSystem )

ADD_FILTER(
src
	DX9RenderSystem.h
	DX9RenderSystem.cpp
	DX9RenderImage.h
	DX9RenderImage.cpp
    DX9RenderTarget.h
    DX9RenderTarget.cpp
	DX9RenderTargetOffscreen.h
	DX9RenderTargetOffscreen.cpp
    DX9RenderVertexBuffer.h
    DX9RenderVertexBuffer.cpp
    DX9RenderVertexAttribute.h
    DX9RenderVertexAttribute.cpp
    DX9RenderIndexBuffer.h
    DX9RenderIndexBuffer.cpp    
    DX9RenderFragmentShader.h
    DX9RenderFragmentShader.cpp
    DX9RenderVertexShader.h
    DX9RenderVertexShader.cpp
    DX9RenderProgram.h
    DX9RenderProgram.cpp
    DX9RenderEnum.h
    DX9ErrorHelper.h
    DX9ErrorHelper.cpp
)

add_definitions(-DUNICODE)

INCLUDE_DIRECTORIES( 
    ${Mengine_SOURCE_DIR}/Utils
    ${THIRDPARTY_DIR}/libmath/include
    d3dx9/include
    )

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER System
)

if(CMAKE_BUILD_TYPE EQUAL "DEBUG")
    SET( APPLICATION_DEPENDENCIES
        ${APPLICATION_DEPENDENCIES}
        ${MY_LIB_NAME}
        ${CMAKE_CURRENT_SOURCE_DIR}/d3dx9/lib/d3dx9d.lib
    )
else()
    SET( APPLICATION_DEPENDENCIES
        ${APPLICATION_DEPENDENCIES}
        ${MY_LIB_NAME}
        ${CMAKE_CURRENT_SOURCE_DIR}/d3dx9/lib/d3dx9.lib
    )
endif()

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)

SET( APPLICATION_LIB_PATHS
 ${APPLICATION_LIB_PATHS}
 PARENT_SCOPE
)

SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}  
)

SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}  
 PARENT_SCOPE
)
