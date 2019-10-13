#include "MySceneEventReceiver.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/CodecServiceInterface.h"

#include "Engine/SurfaceSolidColor.h"
#include "Engine/ShapeQuadFixed.h"
#include "Engine/Vectorizator.h"
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

#include "Config/Vector.h"

#include "GOAP/Source.h"

#include "Tasks/TaskPrint.h"
#include "Tasks/TaskGlobalMouseButton.h"
#include "Tasks/TaskPickerableMouseButton.h"
#include "Tasks/TaskLocalDelay.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MySceneEventReceiver::MySceneEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MySceneEventReceiver::~MySceneEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MySceneEventReceiver::onSceneAppMouseLeave( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onSceneAppMouseLeave [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MySceneEventReceiver::onSceneAppMouseEnter( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onSceneAppMouseEnter [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MySceneEventReceiver::onSceneAppFocus( const EntityBehaviorInterfacePtr & _behavior, bool _focus )
    {
        MENGINE_UNUSED( _behavior );
        MENGINE_UNUSED( _focus );

        LOGGER_MESSAGE( "Scene onSceneAppFocus [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MySceneEventReceiver::onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity )
    {
        MENGINE_UNUSED( _behavior );

        m_scene = Helper::staticNodeCast<Scene *>( _entity );

        LOGGER_MESSAGE( "Scene onEntityCreate [%s]"
            , m_scene->getName().c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    void MySceneEventReceiver::onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityDestroy [%s]"
            , m_scene->getName().c_str()
        );

        m_scene = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void MySceneEventReceiver::onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityActivate [%s]"
            , m_scene->getName().c_str()
        );

        this->setupGame();
    }
    //////////////////////////////////////////////////////////////////////////
    void MySceneEventReceiver::onEntityActivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityActivate [%s]"
            , m_scene->getName().c_str()
        );

        this->runTaskChains();
    }
    //////////////////////////////////////////////////////////////////////////
    void MySceneEventReceiver::onEntityPreparationDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityPreparationDeactivate [%s]"
            , m_scene->getName().c_str()
        );

        this->clearTaskChain();
    }
    //////////////////////////////////////////////////////////////////////////
    void MySceneEventReceiver::onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );
        
        LOGGER_MESSAGE( "Scene onEntityDeactivate [%s]"
            , m_scene->getName().c_str()
        );
        
        this->clearSprites();
        this->clearHotspots();
        this->clearGameNode();
    }
    //////////////////////////////////////////////////////////////////////////
    void MySceneEventReceiver::onEntityCompile( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityCompile [%s]"
            , m_scene->getName().c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    void MySceneEventReceiver::onEntityRelease( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityRelease [%s]"
            , m_scene->getName().c_str()
        );
    }
    /////////////////////////////////////////////////////////////////////////
    ShapeQuadFixedPtr MySceneEventReceiver::createSprite( const ConstString & _name, const ResourcePtr & _resource )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _resource, nullptr, "'%s' resource is NULL"
            , _name.c_str()
        );

        SurfaceImagePtr surface = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceImage" ), MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( surface, nullptr );

        surface->setName( _name );
        surface->setResourceImage( _resource );

        ShapeQuadFixedPtr shape = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ShapeQuadFixed" ), MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( shape, nullptr );

        shape->setName( _name );
        shape->setSurface( surface );

        return shape;
    }
    /////////////////////////////////////////////////////////////////////////
    HotSpotPolygonPtr MySceneEventReceiver::createHotSpot( const ConstString & _name, const mt::vec2f & _size )
    {
        HotSpotPolygonPtr hotspot = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "HotSpotPolygon" ), MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( hotspot, nullptr );

        hotspot->setName( _name );

        Polygon polygon;

        polygon.append( { 0.f, 0.f } );
        polygon.append( { _size.x, 0.f } );
        polygon.append( { _size.x, _size.y } );
        polygon.append( { 0.f, _size.y } );

        hotspot->setPolygon( polygon );

        return hotspot;
    }
    /////////////////////////////////////////////////////////////////////////
    ResourcePtr MySceneEventReceiver::createResource( const ConstString & _type )
    {
        ResourcePtr resource = RESOURCE_SERVICE()
            ->generateResource( _type, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( resource, nullptr, "invalid create resource '%s'"
            , _type.c_str()
        );

        return resource;
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceImageDefaultPtr MySceneEventReceiver::createImageResource( const ConstString & _resourceName, const ConstString & _fileGroupName, const FilePath & _filePath, const mt::vec2f & _maxSize )
    {
        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getFileGroup( _fileGroupName );

        const ConstString & codecType = CODEC_SERVICE()
            ->findCodecType( _filePath );

        mt::vec2f maxSize;

        if( _maxSize.x < 0.f || _maxSize.y < 0.f )
        {
            InputStreamInterfacePtr stream = Helper::openInputStreamFile( fileGroup, _filePath, false, MENGINE_DOCUMENT_FUNCTION );

            MENGINE_ASSERTION_MEMORY_PANIC( stream, nullptr );

            ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE()
                ->createDecoderT<ImageDecoderInterfacePtr>( codecType, MENGINE_DOCUMENT_FUNCTION );

            MENGINE_ASSERTION_MEMORY_PANIC( imageDecoder, nullptr );

            if( imageDecoder->prepareData( stream ) == false )
            {
                return nullptr;
            }

            const ImageCodecDataInfo * dataInfo = imageDecoder->getCodecDataInfo();

            maxSize.x = (float)dataInfo->width;
            maxSize.y = (float)dataInfo->height;
        }
        else
        {
            maxSize = _maxSize;
        }

        ResourceImageDefaultPtr resource = RESOURCE_SERVICE()
            ->generateResource( STRINGIZE_STRING_LOCAL( "ResourceImageDefault" ), MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( resource, nullptr );

        resource->setName( _resourceName );

        ContentInterface * content = resource->getContent();

        content->setFilePath( _filePath );
        content->setFileGroup( fileGroup );
        content->setCodecType( codecType );

        resource->setSize( maxSize );

        mt::uv4f uv_image;
        mt::uv4f uv_alpha;

        if( resource->setup( _filePath, ConstString::none(), uv_image, uv_alpha, maxSize ) == false )
        {
            return nullptr;
        }

        return resource;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MySceneEventReceiver::setupGame()
    {
        // create game node
        NodePtr node = PROTOTYPE_GENERATE( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Node" ), MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( node, false );

        node->setName( STRINGIZE_STRING_LOCAL( "Node_Game" ) );

        m_gameNode = node;

        m_scene->addChild( node );

        // setup some settings
        m_gridSize = mt::vec2f( 3, 3 );

        GOAP::EventPtr eventGameOver = GOAP::EventPtr::from( new GOAP::Event() );
        m_semaphoreGameOver = GOAP::SemaphorePtr::from( new GOAP::Semaphore( eventGameOver, 0 ) );

        m_currentPlayer = EPlayerType::EPlayerType_X;

        m_grid.resize( (int32_t)(m_gridSize.x * m_gridSize.y), EPlayerType::EPlayerType_NONE );

        // setup background
        bool setupBackgroundResult = this->setupBackground();

        MENGINE_UNUSED( setupBackgroundResult );

        MENGINE_ASSERTION_FATAL( setupBackgroundResult == true, false );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MySceneEventReceiver::setupBackground()
    {
        // setup resource
        ResourceImageDefaultPtr resourceImage = this->createImageResource( STRINGIZE_STRING_LOCAL( "Background" ),
            STRINGIZE_STRING_LOCAL( "Assets" ), STRINGIZE_FILEPATH_LOCAL( "background.png" ), { -1.f, -1.f } );

        MENGINE_ASSERTION_MEMORY_PANIC( resourceImage, false );

        m_backgroundSize = resourceImage->getMaxSize();

        // setup sprite
        ShapeQuadFixedPtr sprite = this->createSprite( STRINGIZE_STRING_LOCAL( "BG" ), resourceImage );

        MENGINE_ASSERTION_MEMORY_PANIC( sprite, false );

        m_background = sprite;

        m_gameNode->addChild( sprite );

        // setup position
        const Resolution & resolution = APPLICATION_SERVICE()
            ->getContentResolution();

        float resolutionWidth = resolution.getWidthF();
        float resolutionHeight = resolution.getHeightF();

        m_gameNode->setLocalPosition( { (resolutionWidth - m_backgroundSize.x) / 2, (resolutionHeight - m_backgroundSize.y) / 2, 0.f } );

        // create Hotspot
        mt::vec2f hotspotSize( { m_backgroundSize.x / 3.f, m_backgroundSize.y / 3.f } );

        for( int32_t row = 0; row < 3; row++ )
        {
            for( int32_t col = 0; col < 3; col++ )
            {
                ConstString name = Helper::stringizeStringFormat( "HotSpot_%d_%d", row, col );

                HotSpotPolygonPtr hotspot = this->createHotSpot( name, hotspotSize );

                m_gameNode->addChild( hotspot );

                hotspot->setLocalPosition( { col * hotspotSize.x, row * hotspotSize.y, 0.f } );

                m_hotspots.push_back( hotspot );
            }
        }
            
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MySceneEventReceiver::createPlayerSprite( const EPlayerType & _player, int32_t _position )
    {
        FilePath path;

        if( _player == EPlayerType::EPlayerType_X )
        {
            path = STRINGIZE_FILEPATH_LOCAL( "x.png" );
        }
        else if( _player == EPlayerType::EPlayerType_O )
        {
            path = STRINGIZE_FILEPATH_LOCAL( "o.png" );
        }

        // create node
        ResourceImageDefaultPtr resourceImage = this->createImageResource( STRINGIZE_STRING_LOCAL( "Background" ),
            STRINGIZE_STRING_LOCAL( "Assets" ), path, { -1.f, -1.f } );

        MENGINE_ASSERTION_MEMORY_PANIC( resourceImage, false );

        ShapeQuadFixedPtr sprite = this->createSprite( STRINGIZE_STRING_LOCAL( "BG" ), resourceImage );

        MENGINE_ASSERTION_MEMORY_PANIC( sprite, false );

        m_sprites.push_back( sprite );

        m_gameNode->addChild( sprite );

        // setup position
        const mt::vec2f & spriteSize = resourceImage->getMaxSize();

        mt::vec2f cellSize( { m_backgroundSize.x / m_gridSize.x, m_backgroundSize.y / m_gridSize.y} );
        mt::vec2f halfCellSize( { cellSize.x / 2, cellSize.y / 2 } );

        sprite->setLocalOrigin( { spriteSize.x / 2, spriteSize.y / 2, 0.f } );

        int32_t col = _position % (int32_t)m_gridSize.x;
        int32_t row = _position / (int32_t)m_gridSize.y;

        sprite->setLocalPosition( { halfCellSize.x + col * cellSize.x, halfCellSize.y + row * cellSize.y, 0.f } );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MySceneEventReceiver::runTaskChains()
    {
        GOAP::SourcePtr source = GOAP::SourcePtr::from( new GOAP::Source() );

        source->addWhile( [this]( const GOAP::SourcePtr & _scope_while )
        {
            _scope_while->addTask<TaskPrint>( "NEW GAME" );

            auto && [race_play, race_end] = _scope_while->addRace<2>();

            int32_t cellIdx = 0;
            for( auto && [race, hotpspot] : race_play->addParallelZip( m_hotspots ) )
            {
                race->addTask<TaskPickerableMouseButton>( hotpspot, EMouseCode::MC_LBUTTON, true, true, nullptr );

                race->addTask<TaskPrint>( "Click at cell '%d' hotspot", cellIdx );

                race->addFunction( this, &MySceneEventReceiver::makeTurn, cellIdx );
                race->addFunction( this, &MySceneEventReceiver::changePlayer );

                ++cellIdx;
            }

            race_end->addSemaphoreEqual( m_semaphoreGameOver, 1 );

            _scope_while->addTask<TaskPrint>( "GAME OVER" );

            _scope_while->addScope( this, &MySceneEventReceiver::scopeGameOver );

            _scope_while->addFunction( this, &MySceneEventReceiver::resetGame );

            return true;
        } );

        GOAP::ChainPtr tc = GOAP::ChainPtr::from( new GOAP::Chain( source ) );

        tc->run();

        m_tc = tc;
    }
    //////////////////////////////////////////////////////////////////////////
    void MySceneEventReceiver::makeTurn( int32_t _position )
    {
        this->createPlayerSprite( m_currentPlayer, _position );

        m_grid[_position] = m_currentPlayer;

        if( this->isGameOver() == true )
        {
            m_semaphoreGameOver->setValue( 1 );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MySceneEventReceiver::changePlayer()
    {
        if( m_currentPlayer == EPlayerType::EPlayerType_X )
        {
            m_currentPlayer = EPlayerType::EPlayerType_O;
        }
        else
        {
            m_currentPlayer = EPlayerType::EPlayerType_X;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool MySceneEventReceiver::isGameOver()
    {
        bool result = this->checkWin( m_grid[0], m_grid[1], m_grid[2] )
            || this->checkWin( m_grid[3], m_grid[4], m_grid[5] )
            || this->checkWin( m_grid[6], m_grid[7], m_grid[8] )

            || this->checkWin( m_grid[0], m_grid[3], m_grid[6] )
            || this->checkWin( m_grid[1], m_grid[4], m_grid[7] )
            || this->checkWin( m_grid[2], m_grid[5], m_grid[8] )

            || this->checkWin( m_grid[0], m_grid[4], m_grid[8] )
            || this->checkWin( m_grid[6], m_grid[4], m_grid[2] );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MySceneEventReceiver::checkWin( EPlayerType _cell1, EPlayerType _cell2, EPlayerType _cell3 )
    {
        bool result = _cell1 == _cell2 && _cell2 == _cell3 && _cell3 != EPlayerType::EPlayerType_NONE;

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void MySceneEventReceiver::enableSprites( bool _value )
    {
        for( const ShapeQuadFixedPtr & sprite : m_sprites )
        {
            if( _value == true )
            {
                sprite->enable();
            }
            else
            {
                sprite->disable();
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool MySceneEventReceiver::scopeGameOver( const GOAP::SourcePtr _scope )
    {
        float delay = 100.f;
        int32_t count = 3;

        _scope->addFor( count, [this, delay]( const GOAP::SourcePtr & _scope_for, uint32_t _iterator, uint32_t _count )
        {
            MENGINE_UNUSED( _iterator );
            MENGINE_UNUSED( _count );

            _scope_for->addTask<TaskLocalDelay>( delay );
            _scope_for->addFunction( this, &MySceneEventReceiver::enableSprites, false );
            _scope_for->addTask<TaskLocalDelay>( delay );
            _scope_for->addFunction( this, &MySceneEventReceiver::enableSprites, true );

            return true;
        } );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MySceneEventReceiver::resetGame()
    {
        this->clearSprites();

        m_currentPlayer = EPlayerType::EPlayerType_X;

        m_grid.clear();

        m_grid.resize( (int32_t)(m_gridSize.x * m_gridSize.y), EPlayerType::EPlayerType_NONE );

        m_semaphoreGameOver->setValue( 0 );
    }
    //////////////////////////////////////////////////////////////////////////
    void MySceneEventReceiver::clearGameNode()
    {
        if( m_gameNode != nullptr )
        {
            m_gameNode->removeFromParent();
            m_gameNode->release();
            m_gameNode = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MySceneEventReceiver::clearBackground()
    {
        if( m_background != nullptr )
        {
            m_background->removeFromParent();
            m_background->release();
            m_background = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MySceneEventReceiver::clearSprites()
    {
        for( const ShapeQuadFixedPtr & sprite : m_sprites )
        {
            sprite->removeFromParent();
            sprite->release();
        }

        m_sprites.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void MySceneEventReceiver::clearHotspots()
    {
        for( const HotSpotPolygonPtr & hotspot : m_hotspots )
        {
            hotspot->removeFromParent();
            hotspot->release();
        }

        m_hotspots.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void MySceneEventReceiver::clearTaskChain()
    {
        if( m_tc != nullptr )
        {
            m_tc->cancel();
            m_tc = nullptr;
        }
    }
};