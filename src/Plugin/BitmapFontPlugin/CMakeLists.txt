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

#add_definitions(-DMENGE_PLUGIN_DLL)

INCLUDE_DIRECTORIES( 
        ${Mengine_SOURCE_DIR}/Utils 
		${THIRDPARTY_DIR}/utf8/source
        ${THIRDPARTY_DIR}/libmath/include
        ${THIRDPARTY_DIR}/tinyini/include
)

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

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
    Codec
    Archive
    Factory
    Kernel
    )

SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
 PARENT_SCOPE
)