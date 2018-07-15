set( MY_LIB_NAME BitmapFontPlugin )

ADD_FILTER(
src
    BitmapFontPlugin.h
    BitmapFontPlugin.cpp
	BitmapFontInterface.h
    BitmapFont.h
    BitmapFont.cpp
    BitmapGlyph.h
    BitmapGlyph.cpp
	BitmapGlyphService.h
	BitmapGlyphService.cpp
	BitmapFontPrototypeGenerator.h
	BitmapFontPrototypeGenerator.cpp
)

#add_definitions(-DMENGINE_PLUGIN_DLL)

INCLUDE_DIRECTORIES( 
        ${Mengine_SOURCE_DIR}/Utils 
		${THIRDPARTY_DIR}/utf8/source
        ${THIRDPARTY_DIR}/libmath/include
        ${THIRDPARTY_DIR}/tinyini/include
)

ADD_MENGINE_LIBRARY()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Plugins
)

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