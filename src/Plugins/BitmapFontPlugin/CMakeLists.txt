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

INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/stdex/include )
INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/utf8/source )
INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/libmath/include )

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