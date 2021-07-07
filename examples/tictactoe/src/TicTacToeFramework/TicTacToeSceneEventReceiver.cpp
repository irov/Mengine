#include "TicTacToeSceneEventReceiver.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/CodecServiceInterface.h"

#include "Plugins/GOAPPlugin/Tasks/GOAPCook.h"

#include "Kernel/SurfaceSolidColor.h"
#include "Kernel/ShapeQuadFixed.h"
#include "Kernel/SurfaceImage.h"
#include "Kernel/ResourceImageDefault.h"
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
    TicTacToeSceneEventReceiver::TicTacToeSceneEventReceiver()
        : m_gridSize( 0.f, 0.f )
        , m_backgroundSize( 0.f, 0.f )
        , m_currentPlayer( EPlayerType_NONE )
        , m_scene( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TicTacToeSceneEventReceiver::~TicTacToeSceneEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TicTacToeSceneEventReceiver::onSceneAppMouseLeave( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onSceneAppMouseLeave [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TicTacToeSceneEventReceiver::onSceneAppMouseEnter( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onSceneAppMouseEnter [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TicTacToeSceneEventReceiver::onSceneAppFocus( const EntityBehaviorInterfacePtr & _behavior, bool _focus )
    {
        MENGINE_UNUSED( _behavior );
        MENGINE_UNUSED( _focus );

        LOGGER_MESSAGE( "Scene onSceneAppFocus [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TicTacToeSceneEventReceiver::onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity )
    {
        MENGINE_UNUSED( _behavior );

        m_scene = Helper::staticNodeCast<Scene *>( _entity );

        LOGGER_MESSAGE( "Scene onEntityCreate [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TicTacToeSceneEventReceiver::onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityDestroy [%s]"
            , m_scene->getName().c_str()
        );

        m_scene = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TicTacToeSceneEventReceiver::onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityActivate [%s]"
            , m_scene->getName().c_str()
        );

        this->setupGame();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TicTacToeSceneEventReceiver::onEntityActivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityActivate [%s]"
            , m_scene->getName().c_str()
        );

        this->runTaskChains();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TicTacToeSceneEventReceiver::onEntityPreparationDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityPreparationDeactivate [%s]"
            , m_scene->getName().c_str()
        );

        this->clearTaskChain();
    }
    //////////////////////////////////////////////////////////////////////////
    void TicTacToeSceneEventReceiver::onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior )
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
    void TicTacToeSceneEventReceiver::onEntityCompile( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityCompile [%s]"
            , m_scene->getName().c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    void TicTacToeSceneEventReceiver::onEntityRelease( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityRelease [%s]"
            , m_scene->getName().c_str()
        );
    }
    /////////////////////////////////////////////////////////////////////////
    ShapeQuadFixedPtr TicTacToeSceneEventReceiver::createSprite( const ConstString & _name, const ResourcePtr & _resource )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _resource, "'%s' resource is NULL"
            , _name.c_str()
        );

        SurfaceImagePtr surface = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceImage" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( surface );

        surface->setName( _name );
        surface->setResourceImage( _resource );

        ShapeQuadFixedPtr shape = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ShapeQuadFixed" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( shape );

        shape->setName( _name );
        shape->setSurface( surface );

        return shape;
    }
    /////////////////////////////////////////////////////////////////////////
    HotSpotPolygonPtr TicTacToeSceneEventReceiver::createHotSpot( const ConstString & _name, const mt::vec2f & _size )
    {
        HotSpotPolygonPtr hotspot = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "HotSpotPolygon" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( hotspot );

        hotspot->setName( _name );

        Polygon polygon;

        polygon.append( {0.f, 0.f} );
        polygon.append( {_size.x, 0.f} );
        polygon.append( {_size.x, _size.y} );
        polygon.append( {0.f, _size.y} );

        hotspot->setPolygon( polygon );

        return hotspot;
    }
    /////////////////////////////////////////////////////////////////////////
    ResourcePtr TicTacToeSceneEventReceiver::createResource( const ConstString & _type )
    {
        ResourcePtr resource = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), _type, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( resource, "invalid create resource '%s'"
            , _type.c_str()
        );

        return resource;
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceImageDefaultPtr TicTacToeSceneEventReceiver::createImageResource( const ConstString & _resourceName, const ConstString & _fileGroupName, const FilePath & _filePath, const mt::vec2f & _maxSize )
    {
        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getFileGroup( _fileGroupName );

        const ConstString & codecType = CODEC_SERVICE()
            ->findCodecType( _filePath );

        mt::vec2f maxSize;

        if( _maxSize.x < 0.f || _maxSize.y < 0.f )
        {
            InputStreamInterfacePtr stream = Helper::openInputStreamFile( fileGroup, _filePath, false, false, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( stream );

            ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE()
                ->createDecoderT<ImageDecoderInterfacePtr>( codecType, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( imageDecoder );

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

        ResourceImageDefaultPtr resource = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceImageDefault" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( resource );

        resource->setName( _resourceName );

        ContentInterfacePtr content = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "FileContent" ), ConstString::none(), MENGINE_DOCUMENT_FACTORABLE );

        content->setFileGroup( fileGroup );
        content->setFilePath( _filePath );
        content->setCodecType( codecType );

        resource->setContent( content );

        mt::uv4f uv_image;
        mt::uv4f uv_alpha;
        resource->setUVImage( uv_image );
        resource->setUVAlpha( uv_alpha );

        resource->setMaxSize( maxSize );
        resource->setSize( maxSize );

        if( resource->initialize() == false )
        {
            return nullptr;
        }

        return resource;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TicTacToeSceneEventReceiver::setupGame()
    {
        // create game node
        NodePtr node = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Interender" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( node );

        node->setName( STRINGIZE_STRING_LOCAL( "Node_Game" ) );

        m_gameNode = node;

        m_scene->addChild( node );

        // setup some settings
        m_gridSize = mt::vec2f( 3, 3 );

        m_semaphoreGameOver = GOAP_SERVICE()
            ->makeSemaphore( 0 );

        m_currentPlayer = EPlayerType::EPlayerType_X;

        m_grid.resize( (int32_t)(m_gridSize.x * m_gridSize.y), EPlayerType::EPlayerType_NONE );

        // setup background
        bool setupBackgroundResult = this->setupBackground();

        MENGINE_UNUSED( setupBackgroundResult );

        MENGINE_ASSERTION_FATAL( setupBackgroundResult == true );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TicTacToeSceneEventReceiver::setupBackground()
    {
        // setup resource
        ResourceImageDefaultPtr resourceImage = this->createImageResource( STRINGIZE_STRING_LOCAL( "Background" ),
            STRINGIZE_STRING_LOCAL( "Assets" ), STRINGIZE_FILEPATH_LOCAL( "background.png" ), {-1.f, -1.f} );

        MENGINE_ASSERTION_MEMORY_PANIC( resourceImage );

        m_backgroundSize = resourceImage->getMaxSize();

        // setup sprite
        ShapeQuadFixedPtr sprite = this->createSprite( STRINGIZE_STRING_LOCAL( "BG" ), resourceImage );

        MENGINE_ASSERTION_MEMORY_PANIC( sprite );

        m_background = sprite;

        m_gameNode->addChild( sprite );

        // setup position
        const Resolution & resolution = APPLICATION_SERVICE()
            ->getContentResolution();

        float resolutionWidth = resolution.getWidthF();
        float resolutionHeight = resolution.getHeightF();

        TransformationInterface * gameNodeTransformation = m_gameNode->getTransformation();
        gameNodeTransformation->setLocalPosition( {(resolutionWidth - m_backgroundSize.x) / 2, (resolutionHeight - m_backgroundSize.y) / 2, 0.f} );

        // create Hotspot
        mt::vec2f hotspotSize( {m_backgroundSize.x / 3.f, m_backgroundSize.y / 3.f} );

        for( int32_t row = 0; row < 3; row++ )
        {
            for( int32_t col = 0; col < 3; col++ )
            {
                ConstString name = Helper::stringizeStringFormat( "HotSpot_%d_%d", row, col );

                HotSpotPolygonPtr hotspot = this->createHotSpot( name, hotspotSize );

                m_gameNode->addChild( hotspot );

                TransformationInterface * hotspotTransformation = hotspot->getTransformation();
                hotspotTransformation->setLocalPosition( {col * hotspotSize.x, row * hotspotSize.y, 0.f} );

                m_hotspots.push_back( hotspot );
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TicTacToeSceneEventReceiver::createPlayerSprite( EPlayerType _player, int32_t _position )
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
            STRINGIZE_STRING_LOCAL( "Assets" ), path, {-1.f, -1.f} );

        MENGINE_ASSERTION_MEMORY_PANIC( resourceImage );

        ShapeQuadFixedPtr sprite = this->createSprite( STRINGIZE_STRING_LOCAL( "BG" ), resourceImage );

        MENGINE_ASSERTION_MEMORY_PANIC( sprite );

        m_sprites.push_back( sprite );

        m_gameNode->addChild( sprite );

        // setup position
        const mt::vec2f & spriteSize = resourceImage->getMaxSize();

        mt::vec2f cellSize( {m_backgroundSize.x / m_gridSize.x, m_backgroundSize.y / m_gridSize.y} );
        mt::vec2f halfCellSize( {cellSize.x * 0.5f, cellSize.y * 0.5f} );

        TransformationInterface * spriteTransformation = sprite->getTransformation();

        spriteTransformation->setLocalOrigin( {spriteSize.x * 0.5f, spriteSize.y * 0.5f, 0.f} );

        int32_t col = _position % (int32_t)m_gridSize.x;
        int32_t row = _position / (int32_t)m_gridSize.y;

        spriteTransformation->setLocalPosition( {halfCellSize.x + col * cellSize.x, halfCellSize.y + row * cellSize.y, 0.f} );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TicTacToeSceneEventReceiver::runTaskChains()
    {
        GOAP::SourceInterfacePtr source = GOAP_SERVICE()
            ->makeSource();

        Cook::addWhile( source, [this]( const GOAP::SourceInterfacePtr & _scope_while )
        {
            Cook::addPrint( _scope_while, "NEW GAME" );

            auto && [race_play, race_end] = Cook::addRace<2>( _scope_while );

            int32_t cellIdx = 0;
            for( auto && [race, hotpspot] : Cook::addParallelZip( race_play, m_hotspots ) )
            {
                Cook::addPickerableMouseButton( race, hotpspot, EMouseCode::MC_LBUTTON, true, true, nullptr, MENGINE_DOCUMENT_FACTORABLE );

                Cook::addPrint( race, "Click at cell '%d' hotspot", cellIdx );

                Cook::addFunction( race, this, &TicTacToeSceneEventReceiver::makeTurn, cellIdx );
                Cook::addFunction( race, this, &TicTacToeSceneEventReceiver::changePlayer );

                ++cellIdx;
            }

            Cook::addSemaphoreEqual( race_end, m_semaphoreGameOver, 1 );

            Cook::addPrint( _scope_while, "GAME OVER" );

            Cook::addScope( _scope_while, this, &TicTacToeSceneEventReceiver::scopeGameOver );
            Cook::addFunction( _scope_while, this, &TicTacToeSceneEventReceiver::resetGame );

            return true;
        } );

        GOAP::ChainInterfacePtr chain = GOAP_SERVICE()
            ->makeChain( source, nullptr, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE );

        chain->run();

        m_chain = chain;
    }
    //////////////////////////////////////////////////////////////////////////
    void TicTacToeSceneEventReceiver::makeTurn( int32_t _position )
    {
        this->createPlayerSprite( m_currentPlayer, _position );

        m_grid[_position] = m_currentPlayer;

        if( this->isGameOver() == true )
        {
            m_semaphoreGameOver->setValue( 1 );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void TicTacToeSceneEventReceiver::changePlayer()
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
    bool TicTacToeSceneEventReceiver::isGameOver()
    {
        bool result = this->checkWin( 0, 1, 2 )
            || this->checkWin( 3, 4, 5 )
            || this->checkWin( 6, 7, 8 )
            || this->checkWin( 0, 3, 6 )
            || this->checkWin( 1, 4, 7 )
            || this->checkWin( 2, 5, 8 )
            || this->checkWin( 0, 4, 8 )
            || this->checkWin( 6, 4, 2 );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TicTacToeSceneEventReceiver::checkWin( uint32_t _cell1, uint32_t _cell2, uint32_t _cell3 )
    {
        bool result = m_grid[_cell1] == m_grid[_cell2] && m_grid[_cell2] == m_grid[_cell3] && m_grid[_cell3] != EPlayerType::EPlayerType_NONE;

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void TicTacToeSceneEventReceiver::enableSprites( bool _value )
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
    bool TicTacToeSceneEventReceiver::scopeGameOver( const GOAP::SourceInterfacePtr & _scope )
    {
        float delay = 100.f;
        int32_t count = 3;

        Cook::addFor( _scope, count, [this, delay]( const GOAP::SourceInterfacePtr & _scope_for, uint32_t _iterator, uint32_t _count )
        {
            MENGINE_UNUSED( _iterator );
            MENGINE_UNUSED( _count );

            Cook::addLocalDelay( _scope_for, delay, MENGINE_DOCUMENT_FACTORABLE );
            Cook::addFunction( _scope_for, this, &TicTacToeSceneEventReceiver::enableSprites, false );
            Cook::addLocalDelay( _scope_for, delay, MENGINE_DOCUMENT_FACTORABLE );
            Cook::addFunction( _scope_for, this, &TicTacToeSceneEventReceiver::enableSprites, true );

            return true;
        } );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TicTacToeSceneEventReceiver::resetGame()
    {
        this->clearSprites();

        m_currentPlayer = EPlayerType::EPlayerType_X;

        m_grid.clear();

        m_grid.resize( (int32_t)(m_gridSize.x * m_gridSize.y), EPlayerType::EPlayerType_NONE );

        m_semaphoreGameOver->setValue( 0 );
    }
    //////////////////////////////////////////////////////////////////////////
    void TicTacToeSceneEventReceiver::clearGameNode()
    {
        if( m_gameNode != nullptr )
        {
            m_gameNode->removeFromParent();
            m_gameNode = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void TicTacToeSceneEventReceiver::clearBackground()
    {
        if( m_background != nullptr )
        {
            m_background->removeFromParent();
            m_background = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void TicTacToeSceneEventReceiver::clearSprites()
    {
        for( const ShapeQuadFixedPtr & sprite : m_sprites )
        {
            sprite->removeFromParent();
        }

        m_sprites.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void TicTacToeSceneEventReceiver::clearHotspots()
    {
        for( const HotSpotPolygonPtr & hotspot : m_hotspots )
        {
            hotspot->removeFromParent();
        }

        m_hotspots.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void TicTacToeSceneEventReceiver::clearTaskChain()
    {
        if( m_chain != nullptr )
        {
            m_chain->cancel();
            m_chain = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
};