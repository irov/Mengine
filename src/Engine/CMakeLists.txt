set( MY_LIB_NAME Engine )

ADD_FILTER(
Application
	Application.cpp
	Application.h
)

SOURCE_GROUP( Engine )

ADD_FILTER(
Layer2D
	Layer2D.cpp
	Layer2D.h
)

ADD_FILTER(
Motor\\\\Motor
	Motor.cpp
	Motor.h
)

SOURCE_GROUP( Surface )

ADD_FILTER(
Surface\\\\SurfaceSound
	SurfaceSound.cpp
	SurfaceSound.h
)

ADD_FILTER(
Surface\\\\SurfaceSolidColor
	SurfaceSolidColor.cpp
	SurfaceSolidColor.h
)

ADD_FILTER(
Surface\\\\SurfaceImage
	SurfaceImage.cpp
	SurfaceImage.h
)

ADD_FILTER(
Surface\\\\SurfaceImageSequence
	SurfaceImageSequence.cpp
	SurfaceImageSequence.h
)

ADD_FILTER(
Surface\\\\SurfaceTrackMatte
	SurfaceTrackMatte.cpp
	SurfaceTrackMatte.h
)

SOURCE_GROUP( Object )

ADD_FILTER(Shape\\\\ShapePacMan
    ShapePacMan.cpp
    ShapePacMan.h
)

ADD_FILTER(
Shape\\\\ShapeQuad
	ShapeQuad.cpp
	ShapeQuad.h
	ShapeQuadFixed.cpp
	ShapeQuadFixed.h
	ShapeQuadFlex.cpp
	ShapeQuadFlex.h
)


SOURCE_GROUP( Object )

ADD_FILTER(
Object\\\\HotSpot
	HotSpot.cpp
	HotSpot.h
    HotSpotEventReceiverInterface.h
	HotSpotPolygon.cpp
	HotSpotPolygon.h
	HotSpotImage.cpp
	HotSpotImage.h
	HotSpotBubbles.cpp
	HotSpotBubbles.h
	HotSpotShape.cpp
	HotSpotShape.h
    HotSpotCircle.cpp
    HotSpotCircle.h    
)

ADD_FILTER(
Object\\\\Point
	Point.cpp
	Point.h
)

ADD_FILTER(
Object\\\\Line
	Line.cpp
	Line.h
)


ADD_FILTER(
Object\\\\SoundEmitter
	SoundEmitter.cpp
	SoundEmitter.h
)

ADD_FILTER(
Object\\\\Landscape2D
	Landscape2D.cpp
	Landscape2D.h
)

ADD_FILTER(
Object\\\\Meshget
	Meshget.cpp
	Meshget.h
)


ADD_FILTER(
Object\\\\Grid2D
	Grid2D.cpp
	Grid2D.h
)

ADD_FILTER(
Object\\\\Gyroscope
	Gyroscope.cpp
	Gyroscope.h
)

ADD_FILTER(
Object\\\\TextField
	TextField.cpp
	TextField.h
	TextLine.cpp
	TextLine.h
    TextChar.cpp
    TextChar.h
)

ADD_FILTER(
Object\\\\Window
	Window.cpp
	Window.h
)

ADD_FILTER(
Resource\\\\File
	ResourceFile.cpp
	ResourceFile.h
)

ADD_FILTER(
Resource\\\\ImageData
	ResourceImageData.cpp
	ResourceImageData.h
)

ADD_FILTER(
Resource\\\\ImageSequence
	ResourceImageSequence.cpp
	ResourceImageSequence.h
)

ADD_FILTER(
Resource\\\\ResourceMusic
	ResourceMusic.h
	ResourceMusic.cpp
)	

ADD_FILTER(
Resource\\\\Cursor
	ResourceCursor.h
	ResourceCursorICO.cpp
	ResourceCursorICO.h
    ResourceCursorSystem.cpp
    ResourceCursorSystem.h
)

ADD_FILTER(
Resource\\\\Image
	ResourceImageSolid.h
    ResourceImageSolid.cpp
	ResourceImageDefault.cpp
	ResourceImageDefault.h
	ResourceImageSubstract.cpp
	ResourceImageSubstract.h
	ResourceImageSubstractRGBAndAlpha.cpp
	ResourceImageSubstractRGBAndAlpha.h
)

ADD_FILTER(
Resource\\\\Sound
	ResourceSound.cpp
	ResourceSound.h
)

ADD_FILTER(
Resource\\\\Shape
	ResourceShape.cpp
	ResourceShape.h
)

ADD_FILTER(
Resource\\\\Window
	ResourceWindow.cpp
	ResourceWindow.h
)

ADD_FILTER(
Resource\\\\TestPick
	ResourceTestPick.h
)

ADD_FILTER(
Resource\\\\HIT
	ResourceHIT.cpp
	ResourceHIT.h
)

INCLUDE_DIRECTORIES(
    ${THIRDPARTY_DIR}
    ${THIRDPARTY_DIR}/pybind/include
    ${THIRDPARTY_DIR}/utf8/source
    ${THIRDPARTY_DIR}/tinyini/include
    ${THIRDPARTY_DIR}/libmath/include
)

ADD_MENGINE_LIBRARY()

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}  
 Kernel
 Metacode
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)

SET( APPLICATION_LIBS
    ${APPLICATION_LIBS}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}zlib${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}tinyini${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX}
    )
    
SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
 PARENT_SCOPE
)
