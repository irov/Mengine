MENGINE_PROJECT(Engine)

ADD_FILTER(
Engine
    Engine.h
)

ADD_FILTER(
Generators
    NodeGenerators.h
    NodeGenerators.cpp
    SurfaceGenerators.h
    SurfaceGenerators.cpp
)

ADD_FILTER(
Application
    Application.cpp
    Application.h
    EntityPrototypeGenerator.cpp
    EntityPrototypeGenerator.h
    ScenePrototypeGenerator.cpp
    ScenePrototypeGenerator.h
)

ADD_FILTER(
Layer2D
    Layer2D.cpp
    Layer2D.h
    Layer2DRenderTarget.cpp
    Layer2DRenderTarget.h
)

ADD_FILTER(
Motor
    Motor.cpp
    Motor.h
)

ADD_FILTER(
Object\\\\Isometric
    Isometric.cpp
    Isometric.h
)

ADD_FILTER(
Object\\\\HotSpot
    HotSpotPolygon.cpp
    HotSpotPolygon.h
    HotSpotImage.cpp
    HotSpotImage.h
    HotSpotResourceShape.cpp
    HotSpotResourceShape.h
    HotSpotSurface.cpp
    HotSpotSurface.h
    HotSpotCircle.cpp
    HotSpotCircle.h
    HotSpotGlobal.cpp
    HotSpotGlobal.h
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
    TextRenderChunk.h
    TextDebug.cpp
    TextDebug.h
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

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/utf8/source)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/graphics/include)

ADD_MENGINE_LIBRARY()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Metacode)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}zlib${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}graphics${MENGINE_LIB_SUFFIX})