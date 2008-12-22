
ADD_FILTER(
src
	graphics.cpp
	hge.def
	hge.h
	hge_impl.h
	system.cpp
	HGERenderFont.cpp
	HGERenderFont.h
	HGETexture.cpp
	HGETexture.h
	HGERenderSystem.cpp
	HGERenderSystem.h
)

ADD_LIBRARY( HGERenderSystem ${SRC_FILES} )

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 HGERenderSystem
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)