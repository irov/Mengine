#include "TrafficJamSceneEventReceiver.h"
#include "TrafficMap.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/CodecServiceInterface.h"

#include "Plugins/GOAPPlugin/Tasks/GOAPCook.h"

#include "Engine/SurfaceSolidColor.h"
#include "Engine/ShapeQuadFixed.h"
#include "Engine/SurfaceImage.h"
#include "Engine/ResourceImageDefault.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/Surface.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/StringHelper.h"
#include "Kernel/EntityHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/SchedulerHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static const Color BG_COLOR( 128.f, 128.f, 128.f );
    static const mt::vec2f BG_SIZE( 715, 715 );

    static MENGINE_CONSTEXPR int32_t BORDER_INDENT = 25;
    static MENGINE_CONSTEXPR int32_t PLAYER_CAR_ID = 2;

    static MENGINE_CONSTEXPR int32_t HORIZONTAL_CAR_ID = 0;
    static MENGINE_CONSTEXPR int32_t VERTICAL_CAR_ID = 1;

    static const Color PLAYER_COLOR( 255.f, 0.f, 0.f );
    static const mt::vec2f PLAYER_SIZE( 110, 225 );

    static const Color HORIZONTAL_CAR_COLOR( 0.f, 255.f, 0.f );
    static const mt::vec2f HORIZONTAL_CAR_SIZE( 225, 110 );

    static const Color VERTICAL_CAR_COLOR( 0.f, 0.f, 255.f );
    static const mt::vec2f VERTICAL_CAR_SIZE( 110, 225 );

    static MENGINE_CONSTEXPR float CELL_IN_PIXELS = 115;
    //////////////////////////////////////////////////////////////////////////
    TrafficJamSceneEventReceiver::TrafficJamSceneEventReceiver()
        : m_backgroundSize( 0.f, 0.f )
        , m_scene( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TrafficJamSceneEventReceiver::~TrafficJamSceneEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TrafficJamSceneEventReceiver::onSceneAppMouseLeave( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onSceneAppMouseLeave [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TrafficJamSceneEventReceiver::onSceneAppMouseEnter( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onSceneAppMouseEnter [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TrafficJamSceneEventReceiver::onSceneAppFocus( const EntityBehaviorInterfacePtr & _behavior, bool _focus )
    {
        MENGINE_UNUSED( _behavior );
        MENGINE_UNUSED( _focus );

        LOGGER_MESSAGE( "Scene onSceneAppFocus [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TrafficJamSceneEventReceiver::onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity )
    {
        MENGINE_UNUSED( _behavior );

        m_scene = Helper::staticNodeCast<Scene *>( _entity );

        LOGGER_MESSAGE( "Scene onEntityCreate [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TrafficJamSceneEventReceiver::onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityDestroy [%s]"
            , m_scene->getName().c_str()
        );

        m_scene = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TrafficJamSceneEventReceiver::onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityPreparation [%s]"
            , m_scene->getName().c_str()
        );

        this->setupGame();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TrafficJamSceneEventReceiver::onEntityActivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityActivate [%s]"
            , m_scene->getName().c_str()
        );

        this->runTaskChains();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TrafficJamSceneEventReceiver::onEntityPreparationDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityPreparationDeactivate [%s]"
            , m_scene->getName().c_str()
        );

        this->clearTaskChain();
    }
    //////////////////////////////////////////////////////////////////////////
    void TrafficJamSceneEventReceiver::onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityDeactivate [%s]"
            , m_scene->getName().c_str()
        );

        this->clearGameNode();
    }
    //////////////////////////////////////////////////////////////////////////
    void TrafficJamSceneEventReceiver::onEntityCompile( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityCompile [%s]"
            , m_scene->getName().c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    void TrafficJamSceneEventReceiver::onEntityRelease( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityRelease [%s]"
            , m_scene->getName().c_str()
        );
    }
    /////////////////////////////////////////////////////////////////////////
    ShapeQuadFixedPtr TrafficJamSceneEventReceiver::createColor( const ConstString & _name, const SurfaceSolidColorPtr & _resource )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _resource, "'%s' resource is NULL"
            , _name.c_str()
        );

        ShapeQuadFixedPtr shape = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ShapeQuadFixed" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( shape );

        shape->setName( _name );
        shape->setSurface( _resource );

        return shape;
    }
    /////////////////////////////////////////////////////////////////////////
    HotSpotPolygonPtr TrafficJamSceneEventReceiver::createHotSpot( const ConstString & _name, const mt::vec2f & _size )
    {
        HotSpotPolygonPtr hotspot = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "HotSpotPolygon" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( hotspot );

        hotspot->setName( _name );

        Polygon polygon;

        polygon.append( { 0.f, 0.f } );
        polygon.append( { _size.x, 0.f } );
        polygon.append( { _size.x, _size.y } );
        polygon.append( { 0.f, _size.y } );

        hotspot->setPolygon( polygon );

        hotspot->enable();

        return hotspot;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TrafficJamSceneEventReceiver::setupGame()
    {
        // create game node
        NodePtr node = PROTOTYPE_GENERATE( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Node" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( node );

        node->setName( STRINGIZE_STRING_LOCAL( "Node_Game" ) );

        m_gameNode = node;

        m_scene->addChild( node );

        // setup some settings

        m_semaphoreGameOver = GOAP_SERVICE()
            ->makeSemaphore( 0 );

        // setup background
        bool setupBackgroundResult = this->setupBackground();

        MENGINE_ASSERTION_FATAL( setupBackgroundResult == true );

        m_trafficMap.initMap();

        bool setupCars = this->setupCars();

        MENGINE_UNUSED( setupCars );

        MENGINE_ASSERTION_FATAL( setupCars == true );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TrafficJamSceneEventReceiver::setupBackground()
    {
        SurfaceSolidColorPtr colorSolid = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceSolidColor" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( colorSolid );

        colorSolid->setSolidColor( BG_COLOR );
        colorSolid->setSolidSize( BG_SIZE );

        m_backgroundSize = BG_SIZE;

        // setup sprite
        ShapeQuadFixedPtr sprite = this->createColor( STRINGIZE_STRING_LOCAL( "BG" ), colorSolid );

        MENGINE_ASSERTION_MEMORY_PANIC( sprite );

        m_background = sprite;

        m_gameNode->addChild( sprite );

        // setup position
        const Resolution & resolution = APPLICATION_SERVICE()
            ->getContentResolution();

        float resolutionWidth = (resolution.getWidthF() - m_backgroundSize.x) / 2;
        float resolutionHeight = (resolution.getHeightF() - m_backgroundSize.y) / 2;

        m_gameNode->setLocalPosition( { resolutionWidth, resolutionHeight, 0.f } );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TrafficJamSceneEventReceiver::setupCars()
    {
        const RandomizerInterfacePtr & randomizer = PLAYER_SERVICE()
            ->getRandomizer();

        int32_t mapSelector = randomizer->getRandomRangei( 1, 4 );

        switch( mapSelector )
        {
        case 1: //setupCar(cellX, cellY)
            {
                this->setupCar( 3, 0, PLAYER_CAR_ID );
                this->setupCar( 0, 0, HORIZONTAL_CAR_ID );
                this->setupCar( 4, 1, HORIZONTAL_CAR_ID );
                this->setupCar( 2, 2, HORIZONTAL_CAR_ID );
                this->setupCar( 4, 2, HORIZONTAL_CAR_ID );
                this->setupCar( 0, 3, HORIZONTAL_CAR_ID );
                this->setupCar( 2, 3, HORIZONTAL_CAR_ID );
                this->setupCar( 0, 4, HORIZONTAL_CAR_ID );
                this->setupCar( 4, 5, HORIZONTAL_CAR_ID );
                this->setupCar( 2, 0, VERTICAL_CAR_ID );
                this->setupCar( 1, 1, VERTICAL_CAR_ID );
                this->setupCar( 5, 3, VERTICAL_CAR_ID );

                break;
            }
        case 2:
            {
                this->setupCar( 3, 0, PLAYER_CAR_ID );
                this->setupCar( 1, 0, HORIZONTAL_CAR_ID );
                this->setupCar( 4, 1, HORIZONTAL_CAR_ID );
                this->setupCar( 3, 2, HORIZONTAL_CAR_ID );
                this->setupCar( 1, 3, HORIZONTAL_CAR_ID );
                this->setupCar( 0, 4, HORIZONTAL_CAR_ID );
                this->setupCar( 0, 5, HORIZONTAL_CAR_ID );
                this->setupCar( 3, 5, HORIZONTAL_CAR_ID );
                this->setupCar( 0, 0, VERTICAL_CAR_ID );
                this->setupCar( 1, 1, VERTICAL_CAR_ID );
                this->setupCar( 2, 1, VERTICAL_CAR_ID );
                this->setupCar( 5, 2, VERTICAL_CAR_ID );
                this->setupCar( 2, 4, VERTICAL_CAR_ID );
                this->setupCar( 5, 4, VERTICAL_CAR_ID );

                break;
            }
        case 3:
            {
                this->setupCar( 3, 2, PLAYER_CAR_ID );
                this->setupCar( 0, 2, HORIZONTAL_CAR_ID );
                this->setupCar( 4, 2, HORIZONTAL_CAR_ID );
                this->setupCar( 4, 3, HORIZONTAL_CAR_ID );
                this->setupCar( 2, 4, HORIZONTAL_CAR_ID );
                this->setupCar( 1, 5, HORIZONTAL_CAR_ID );
                this->setupCar( 3, 5, HORIZONTAL_CAR_ID );
                this->setupCar( 2, 0, VERTICAL_CAR_ID );
                this->setupCar( 4, 0, VERTICAL_CAR_ID );
                this->setupCar( 2, 2, VERTICAL_CAR_ID );
                this->setupCar( 1, 3, VERTICAL_CAR_ID );
                this->setupCar( 0, 4, VERTICAL_CAR_ID );
                this->setupCar( 5, 4, VERTICAL_CAR_ID );

                break;
            }
        default:
            break;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    HotSpotPolygonPtr TrafficJamSceneEventReceiver::addHotspot( uint32_t _row, uint32_t _col, uint32_t _id )
    {
        mt::vec2f hotspotSize( { CELL_IN_PIXELS, CELL_IN_PIXELS } );

        ConstString name = Helper::stringizeStringFormat( "HotSpot_%d", _id );

        HotSpotPolygonPtr hotspot = this->createHotSpot( name, hotspotSize );

        hotspot->setLocalPosition( { _row * hotspotSize.x, _col * hotspotSize.y, 0.f } );

        return hotspot;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TrafficJamSceneEventReceiver::setupCar( uint32_t _row, uint32_t _col, uint32_t _carID )
    {
        static int carId = 1;

        Car newCar;

        ConstString name = Helper::stringizeStringFormat( "Car_%d", carId );

        ShapeQuadFixedPtr sprite;

        if( _carID == PLAYER_CAR_ID )
        {
            SurfaceSolidColorPtr colorSolid = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceSolidColor" ), MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( colorSolid );

            colorSolid->setSolidColor( PLAYER_COLOR );
            colorSolid->setSolidSize( PLAYER_SIZE );

            sprite = this->createColor( name, colorSolid );

            MENGINE_ASSERTION_MEMORY_PANIC( sprite );
            //create color and chose right size
            newCar.carType = _carID;

            m_trafficMap.setCell( _row, _col, ECellType::ECellType_PLAYER_V_UP );
            newCar.hotspotFirst = addHotspot( 0, 0, ECellType::ECellType_PLAYER_V_UP );

            m_trafficMap.setCell( _row, _col + 1, ECellType::ECellType_PLAYER_V_DOWN );
            newCar.hotspotSecond = addHotspot( 0, 1, ECellType::ECellType_PLAYER_V_DOWN );

            sprite->addChild( newCar.hotspotFirst );
            sprite->addChild( newCar.hotspotSecond );
        }
        else if( _carID == HORIZONTAL_CAR_ID )
        {
            SurfaceSolidColorPtr colorSolid = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceSolidColor" ), MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( colorSolid );

            colorSolid->setSolidColor( HORIZONTAL_CAR_COLOR );
            colorSolid->setSolidSize( HORIZONTAL_CAR_SIZE );

            sprite = this->createColor( name, colorSolid );

            newCar.carType = _carID;

            m_trafficMap.setCell( _row, _col, ECellType::ECellType_HORIZONTAL_LEFT );
            newCar.hotspotFirst = addHotspot( 0, 0, ECellType::ECellType_HORIZONTAL_LEFT );

            m_trafficMap.setCell( _row + 1, _col, ECellType::ECellType_HORIZONTAL_RIGHT );
            newCar.hotspotSecond = addHotspot( 1, 0, ECellType::ECellType_HORIZONTAL_RIGHT );

            sprite->addChild( newCar.hotspotFirst );
            sprite->addChild( newCar.hotspotSecond );
        }
        else if( _carID == VERTICAL_CAR_ID )
        {
            SurfaceSolidColorPtr colorSolid = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceSolidColor" ), MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( colorSolid );

            colorSolid->setSolidColor( VERTICAL_CAR_COLOR );
            colorSolid->setSolidSize( VERTICAL_CAR_SIZE );

            sprite = this->createColor( name, colorSolid );

            newCar.carType = _carID;

            m_trafficMap.setCell( _row, _col, ECellType::ECellType_VERTICAL_UP );
            newCar.hotspotFirst = addHotspot( 0, 0, ECellType::ECellType_VERTICAL_UP );

            m_trafficMap.setCell( _row, _col + 1, ECellType::ECellType_VERTICAL_DOWN );
            newCar.hotspotSecond = addHotspot( 0, 1, ECellType::ECellType_VERTICAL_DOWN );

            sprite->addChild( newCar.hotspotFirst );
            sprite->addChild( newCar.hotspotSecond );
        }

        newCar.sprite = sprite;

        m_gameNode->addChild( sprite );

        const mt::vec3f spriteLocalPosition = { BORDER_INDENT + _row * CELL_IN_PIXELS, BORDER_INDENT + _col * CELL_IN_PIXELS, 0.f };

        sprite->setLocalPosition( spriteLocalPosition );

        m_cars.push_back( newCar );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TrafficJamSceneEventReceiver::moveLeft( const ShapeQuadFixedPtr & _sprite )
    {
        const mt::vec3f & pos = _sprite->getLocalPosition();

        int32_t x_pos = (static_cast<int32_t>(pos.x - BORDER_INDENT)) / static_cast<int32_t>(CELL_IN_PIXELS);
        int32_t y_pos = (static_cast<int32_t>(pos.y - BORDER_INDENT)) / static_cast<int32_t>(CELL_IN_PIXELS);

        ECellType carType = m_trafficMap.getCell( x_pos - 1, y_pos );

        if( x_pos > 0 && carType == ECellType::ECellType_EMPTY )
        {
            _sprite->setLocalPositionX( pos.x - CELL_IN_PIXELS );
            m_trafficMap.setCell( x_pos - 1, y_pos, ECellType::ECellType_HORIZONTAL_LEFT );
            m_trafficMap.setCell( x_pos, y_pos, ECellType::ECellType_HORIZONTAL_RIGHT );
            m_trafficMap.setCell( x_pos + 1, y_pos, ECellType::ECellType_EMPTY );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void TrafficJamSceneEventReceiver::moveRight( const ShapeQuadFixedPtr & _sprite )
    {
        const mt::vec3f & pos = _sprite->getLocalPosition();

        int32_t x_pos = (static_cast<int32_t>(pos.x - BORDER_INDENT)) / static_cast<int32_t>(CELL_IN_PIXELS);
        int32_t y_pos = (static_cast<int32_t>(pos.y - BORDER_INDENT)) / static_cast<int32_t>(CELL_IN_PIXELS);

        ECellType carType = m_trafficMap.getCell( x_pos + 2, y_pos );

        if( x_pos < 4 && carType == ECellType::ECellType_EMPTY )
        {
            _sprite->setLocalPositionX( pos.x + CELL_IN_PIXELS );
            m_trafficMap.setCell( x_pos + 1, y_pos, ECellType::ECellType_HORIZONTAL_LEFT );
            m_trafficMap.setCell( x_pos, y_pos, ECellType::ECellType_EMPTY );
            m_trafficMap.setCell( x_pos + 2, y_pos, ECellType::ECellType_HORIZONTAL_RIGHT );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void TrafficJamSceneEventReceiver::moveUp( const ShapeQuadFixedPtr & _sprite )
    {
        const mt::vec3f & pos = _sprite->getLocalPosition();

        int32_t x_pos = (static_cast<int32_t>(pos.x - BORDER_INDENT)) / static_cast<int32_t>(CELL_IN_PIXELS);
        int32_t y_pos = (static_cast<int32_t>(pos.y - BORDER_INDENT)) / static_cast<int32_t>(CELL_IN_PIXELS);

        ECellType carType = m_trafficMap.getCell( x_pos, y_pos - 1 );

        if( y_pos > 0 && carType == ECellType::ECellType_EMPTY )
        {
            _sprite->setLocalPositionY( pos.y - CELL_IN_PIXELS );
            m_trafficMap.setCell( x_pos, y_pos - 1, ECellType::ECellType_VERTICAL_UP );
            m_trafficMap.setCell( x_pos, y_pos, ECellType::ECellType_VERTICAL_DOWN );
            m_trafficMap.setCell( x_pos, y_pos + 1, ECellType::ECellType_EMPTY );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void TrafficJamSceneEventReceiver::moveDown( const ShapeQuadFixedPtr & _sprite )
    {
        const mt::vec3f & pos = _sprite->getLocalPosition();

        int32_t x_pos = (static_cast<int32_t>(pos.x - BORDER_INDENT)) / static_cast<int32_t>(CELL_IN_PIXELS);
        int32_t y_pos = (static_cast<int32_t>(pos.y - BORDER_INDENT)) / static_cast<int32_t>(CELL_IN_PIXELS);

        ECellType carType = m_trafficMap.getCell( x_pos, y_pos + 2 );

        if( y_pos < 4 && carType == ECellType::ECellType_EMPTY )
        {
            _sprite->setLocalPositionY( pos.y + CELL_IN_PIXELS );
            m_trafficMap.setCell( x_pos, y_pos + 1, ECellType::ECellType_VERTICAL_UP );
            m_trafficMap.setCell( x_pos, y_pos, ECellType::ECellType_EMPTY );
            m_trafficMap.setCell( x_pos, y_pos + 2, ECellType::ECellType_VERTICAL_DOWN );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void TrafficJamSceneEventReceiver::movePlayerUp( const ShapeQuadFixedPtr & _sprite )
    {
        const mt::vec3f & pos = _sprite->getLocalPosition();

        int32_t x_pos = (static_cast<int32_t>(pos.x - BORDER_INDENT)) / static_cast<int32_t>(CELL_IN_PIXELS);
        int32_t y_pos = (static_cast<int32_t>(pos.y - BORDER_INDENT)) / static_cast<int32_t>(CELL_IN_PIXELS);

        ECellType carType = m_trafficMap.getCell( x_pos, y_pos - 1 );

        if( y_pos > 0 && carType == ECellType::ECellType_EMPTY )
        {
            _sprite->setLocalPositionY( pos.y - CELL_IN_PIXELS );
            m_trafficMap.setCell( x_pos, y_pos - 1, ECellType::ECellType_VERTICAL_UP );
            m_trafficMap.setCell( x_pos, y_pos, ECellType::ECellType_VERTICAL_DOWN );
            m_trafficMap.setCell( x_pos, y_pos + 1, ECellType::ECellType_EMPTY );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void TrafficJamSceneEventReceiver::movePlayerDown( const ShapeQuadFixedPtr & _sprite )
    {
        const mt::vec3f & pos = _sprite->getLocalPosition();

        int32_t x_pos = (static_cast<int32_t>(pos.x - BORDER_INDENT)) / static_cast<int32_t>(CELL_IN_PIXELS);
        int32_t y_pos = (static_cast<int32_t>(pos.y - BORDER_INDENT)) / static_cast<int32_t>(CELL_IN_PIXELS);

        if( y_pos + 1 > 4 )
        {
            m_semaphoreGameOver->setValue( 1 );
        }
        else if( y_pos < 4 && m_trafficMap.getCell( x_pos, y_pos + 2 ) == ECellType::ECellType_EMPTY )
        {
            _sprite->setLocalPositionY( pos.y + CELL_IN_PIXELS );
            m_trafficMap.setCell( x_pos, y_pos, ECellType::ECellType_EMPTY );
            m_trafficMap.setCell( x_pos, y_pos + 1, ECellType::ECellType_PLAYER_V_UP );
            m_trafficMap.setCell( x_pos, y_pos + 2, ECellType::ECellType_PLAYER_V_DOWN );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void TrafficJamSceneEventReceiver::runTaskChains()
    {
        GOAP::SourceInterfacePtr source = GOAP_SERVICE()
            ->makeSource();

        Cook::addWhile( source, [this]( const GOAP::SourceInterfacePtr & _scope_while )
        {
            Cook::addPrint( _scope_while, "NEW GAME" );

            auto && [race_play, race_end] = Cook::addRace<2>( _scope_while );

            for( auto && [pickable_race, element] : Cook::addRaceZip( race_play, m_cars ) )
            {
                auto && [race_left, race_right] = Cook::addRace<2>( pickable_race );

                Cook::addPickerableMouseButton( race_left, element.hotspotFirst, EMouseCode::MC_LBUTTON, true, true, nullptr );
                Cook::addPickerableMouseButton( race_right, element.hotspotSecond, EMouseCode::MC_LBUTTON, true, true, nullptr );

                if( element.carType == HORIZONTAL_CAR_ID )
                {
                    Cook::addPrint( race_left, "RACE_LEFT - HotSpot_1" );
                    Cook::addPrint( race_right, "RACE_RIGHT - HotSpot_1" );
                    Cook::addFunction( race_left, this, &TrafficJamSceneEventReceiver::moveLeft, element.sprite );
                    Cook::addFunction( race_right, this, &TrafficJamSceneEventReceiver::moveRight, element.sprite );
                }
                else if( element.carType == VERTICAL_CAR_ID )
                {
                    Cook::addPrint( race_left, "RACE_LEFT - HotSpot_3" );
                    Cook::addPrint( race_right, "RACE_RIGHT - HotSpot_3" );
                    Cook::addFunction( race_left, this, &TrafficJamSceneEventReceiver::moveUp, element.sprite );
                    Cook::addFunction( race_right, this, &TrafficJamSceneEventReceiver::moveDown, element.sprite );
                }
                else if( element.carType == PLAYER_CAR_ID )
                {
                    Cook::addPrint( race_left, "RACE_LEFT - HotSpot_5" );
                    Cook::addPrint( race_right, "RACE_RIGHT - HotSpot_5" );
                    Cook::addFunction( race_left, this, &TrafficJamSceneEventReceiver::movePlayerUp, element.sprite );
                    Cook::addFunction( race_right, this, &TrafficJamSceneEventReceiver::movePlayerDown, element.sprite );
                }
            }

            Cook::addSemaphoreEqual( race_end, m_semaphoreGameOver, 1 );

            Cook::addPrint( race_end, "YOU WIN!" );

            Cook::addFunction( race_end, this, &TrafficJamSceneEventReceiver::resetGame );

            return true;
        } );

        m_chain = GOAP_SERVICE()
            ->makeChain( source, nullptr, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE );

        m_chain->run();
    }
    //////////////////////////////////////////////////////////////////////////
    void TrafficJamSceneEventReceiver::clearCars()
    {
        for( Car & car : m_cars )
        {
            car.sprite->removeFromParent();
            car.sprite = nullptr;

            car.hotspotFirst->removeFromParent();
            car.hotspotFirst = nullptr;

            car.hotspotSecond->removeFromParent();
            car.hotspotSecond = nullptr;
        }

        m_cars.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void TrafficJamSceneEventReceiver::resetGame()
    {
        this->clearCars();
        this->clearBackground();
        this->clearTaskChain();
        m_trafficMap.clearMap();

        this->setupGame();
        this->runTaskChains();
    }
    //////////////////////////////////////////////////////////////////////////
    void TrafficJamSceneEventReceiver::clearGameNode()
    {
        if( m_gameNode != nullptr )
        {
            m_gameNode->removeFromParent();
            m_gameNode = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void TrafficJamSceneEventReceiver::clearBackground()
    {
        if( m_background != nullptr )
        {
            m_background->removeFromParent();
            m_background = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void TrafficJamSceneEventReceiver::clearTaskChain()
    {
        if( m_chain != nullptr )
        {
            m_chain->cancel();
            m_chain = nullptr;
        }
    }
};
