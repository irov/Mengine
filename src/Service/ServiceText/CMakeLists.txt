SET( MY_LIB_NAME Service2Text2 )

ADD_FILTER(
src
	TextEntry.cpp
    TextEntry.h
    TextFont.cpp
    TextFont.h
	TextGlyph.cpp
    TextGlyph.h
    TextGlyphChar.cpp
    TextGlyphChar.h
    TextManager.cpp
    TextManager.h
    TextLocalePak.cpp
    TextLocalePak.h
)

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

INCLUDE_DIRECTORIES( 
    ${Mengine_SOURCE_DIR} 
    ${Mengine_SOURCE_DIR}/Utils 
    ${THIRDPARTY_DIR}/utf8/source
    ) 

SET( MENGE_DEPENDENCIES
 ${MENGE_DEPENDENCIES}
 ${MY_LIB_NAME}
)
 
SET( MENGE_DEPENDENCIES
 ${MENGE_DEPENDENCIES}
 PARENT_SCOPE
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)


SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)
