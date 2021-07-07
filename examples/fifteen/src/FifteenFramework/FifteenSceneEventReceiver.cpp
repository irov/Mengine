#include "FifteenSceneEventReceiver.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/RenderTextureInterface.h"

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

#include <algorithm>
#include <random>
#include <chrono>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static MENGINE_CONSTEXPR int32_t GRID_SIZE_X = 2;
    static MENGINE_CONSTEXPR int32_t GRID_SIZE_Y = 2;
    static MENGINE_CONSTEXPR int32_t GRID_LENGTH = GRID_SIZE_Y * GRID_SIZE_X;

    static MENGINE_CONSTEXPR int32_t HIGHLIGHT_COUNT = 3;
    static MENGINE_CONSTEXPR float HIGHLIGHT_INTERVAL = 100.f;

    static MENGINE_CONSTEXPR uint32_t IMAGE_COUNT = 4;
    //////////////////////////////////////////////////////////////////////////
    FifteenSceneEventReceiver::FifteenSceneEventReceiver()
        : m_backgroundSize( 0.f, 0.f )
        , m_sceneSize( 0.f, 0.f )
        , m_minimumDistanceX( 0.f )
        , m_minimumDistanceY( 0.f )
        , m_emptyIndexX( 0 )
        , m_emptyIndexY( 0 )
        , m_imageIndex( 0 )
        , m_scene( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FifteenSceneEventReceiver::~FifteenSceneEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool FifteenSceneEventReceiver::onSceneAppMouseLeave( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onSceneAppMouseLeave [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FifteenSceneEventReceiver::onSceneAppMouseEnter( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onSceneAppMouseEnter [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FifteenSceneEventReceiver::onSceneAppFocus( const EntityBehaviorInterfacePtr & _behavior, bool _focus )
    {
        MENGINE_UNUSED( _behavior );
        MENGINE_UNUSED( _focus );

        LOGGER_MESSAGE( "Scene onSceneAppFocus [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FifteenSceneEventReceiver::onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity )
    {
        MENGINE_UNUSED( _behavior );

        m_scene = Helper::staticNodeCast<Scene *>( _entity );

        LOGGER_MESSAGE( "Scene onEntityCreate [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FifteenSceneEventReceiver::onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityDestroy [%s]"
            , m_scene->getName().c_str()
        );

        m_scene = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FifteenSceneEventReceiver::onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityActivate [%s]"
            , m_scene->getName().c_str()
        );

        this->setupGame( STRINGIZE_STRING_LOCAL( "image_0.jpg" ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FifteenSceneEventReceiver::onEntityActivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityActivate [%s]"
            , m_scene->getName().c_str()
        );

        this->runTaskChains();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FifteenSceneEventReceiver::onEntityPreparationDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityPreparationDeactivate [%s]"
            , m_scene->getName().c_str()
        );

        this->clearTaskChain();
    }
    //////////////////////////////////////////////////////////////////////////
    void FifteenSceneEventReceiver::onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityDeactivate [%s]"
            , m_scene->getName().c_str()
        );

        this->clearSprites();
        this->clearHotspots();
        this->clearGameNode();
        this->clearBackground();
        this->clearMap();
    }
    //////////////////////////////////////////////////////////////////////////
    void FifteenSceneEventReceiver::onEntityCompile( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityCompile [%s]"
            , m_scene->getName().c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    void FifteenSceneEventReceiver::onEntityRelease( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityRelease [%s]"
            , m_scene->getName().c_str()
        );
    }
    /////////////////////////////////////////////////////////////////////////
    ShapeQuadFixedPtr FifteenSceneEventReceiver::createSprite( const ConstString & _name, const ResourcePtr & _resource )
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
    HotSpotPolygonPtr FifteenSceneEventReceiver::createHotSpot( const ConstString & _name, const mt::vec2f & _size )
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
    //////////////////////////////////////////////////////////////////////////
    ResourceImageDefaultPtr FifteenSceneEventReceiver::createImageSubstractResource( const ConstString & _resourceName, const ConstString & _fileGroupName, const FilePath & _filePath, const mt::vec2f & _maxSize, const mt::vec4f & _splitSize )
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

            maxSize.x = static_cast<float>(dataInfo->width);
            maxSize.y = static_cast<float>(dataInfo->height);
        }
        else
        {
            maxSize = _maxSize;
        }

        ResourceImageDefaultPtr resource = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceImageDefault" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( resource );

        resource->setName( _resourceName );

        mt::uv4f uv_image;
        mt::uv4f uv_alpha;

        mt::vec4f uv_image_mask( _splitSize.x, _splitSize.y, _splitSize.z, _splitSize.w );
        mt::uv4_from_mask( uv_image, uv_image_mask );

        mt::vec2f gridSplit = {1.f / GRID_SIZE_Y, 1.f / GRID_SIZE_X};

        maxSize.x *= gridSplit.x;
        maxSize.y *= gridSplit.y;

        ContentInterfacePtr content = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "FileContent" ), ConstString::none(), MENGINE_DOCUMENT_FACTORABLE );

        content->setFileGroup( fileGroup );
        content->setFilePath( _filePath );
        content->setCodecType( codecType );

        resource->setContent( content );

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
    ResourceImageDefaultPtr FifteenSceneEventReceiver::createImageResource( const ConstString & _resourceName, const ConstString & _fileGroupName, const FilePath & _filePath, const mt::vec2f & _maxSize )
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

            maxSize.x = static_cast<float>(dataInfo->width);
            maxSize.y = static_cast<float>(dataInfo->height);
        }
        else
        {
            maxSize = _maxSize;
        }

        ResourceImageDefaultPtr resource = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceImageDefault" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( resource );

        resource->setName( _resourceName );

        mt::uv4f uv_image;
        mt::uv4f uv_alpha;

        ContentInterfacePtr content = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "FileContent" ), ConstString::none(), MENGINE_DOCUMENT_FACTORABLE );

        content->setFileGroup( fileGroup );
        content->setFilePath( _filePath );
        content->setCodecType( codecType );

        resource->setContent( content );

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
    void FifteenSceneEventReceiver::shuffle( GridVector & _vector )
    {
        const RandomizerInterfacePtr randomizer = PLAYER_SERVICE()
            ->getRandomizer();

        for( int32_t row = 0; row != GRID_SIZE_Y; row++ )
        {
            const int32_t randomIndexY = randomizer->getRandomRangei( 0, GRID_SIZE_Y );

            for( int32_t col = 0; col != GRID_SIZE_X; col++ )
            {
                const int32_t randomIndexX = randomizer->getRandomRangei( 0, GRID_SIZE_X );
                std::swap( _vector[row][col], _vector[randomIndexY][randomIndexX] );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void FifteenSceneEventReceiver::calculateEmptyIndex()
    {
        for( int32_t row = 0; row != GRID_SIZE_Y; row++ )
        {
            for( int32_t col = 0; col != GRID_SIZE_X; col++ )
            {
                const int32_t empty = m_map[row][col];

                if( empty == GRID_LENGTH - 1 )
                {
                    m_emptyIndexY = row;
                    m_emptyIndexX = col;
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void FifteenSceneEventReceiver::setupRandomMap()
    {
        const RandomizerInterfacePtr randomizer = PLAYER_SERVICE()
            ->getRandomizer();

        bool solutionExist = false;

        while( !solutionExist )
        {
            this->shuffle( m_map );

            this->calculateEmptyIndex();

            solutionExist = this->checkSolution();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void FifteenSceneEventReceiver::setupHotspots()
    {
        mt::vec2f hotspotSize( {m_sceneSize.x / GRID_SIZE_X, m_sceneSize.y / GRID_SIZE_Y} );

        //// calculate hotspot minimum size
        m_minimumDistanceX = hotspotSize.x;
        m_minimumDistanceY = hotspotSize.y;

        for( int32_t row = 0; row != GRID_SIZE_Y; row++ )
        {
            Vector<HotSpotPolygonPtr> hotspotRow;

            m_hotspots.push_back( hotspotRow );

            for( int32_t col = 0; col != GRID_SIZE_X; col++ )
            {
                ConstString name = Helper::stringizeStringFormat( "HotSpot_%d_%d", row, col );

                HotSpotPolygonPtr hotspot = this->createHotSpot( name, hotspotSize );

                m_gameNode->addChild( hotspot );

                TransformationInterface * transformation = hotspot->getTransformation();
                transformation->setLocalPosition( {col * hotspotSize.x, row * hotspotSize.y, 0.f} );

                m_hotspots[row].push_back( hotspot );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void FifteenSceneEventReceiver::setupSprites()
    {
        for( int32_t row = 0; row != GRID_SIZE_Y; row++ )
        {
            for( int32_t col = 0; col != GRID_SIZE_X; col++ )
            {
                int32_t index = m_map[row][col];
                if( index != GRID_LENGTH - 1 )
                {
                    m_hotspots[row][col]->addChild( m_sprites[index] );
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool FifteenSceneEventReceiver::setupGame( const ConstString & _imagePath )
    {
        // create game node
        NodePtr node = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Interender" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( node );

        node->setName( STRINGIZE_STRING_LOCAL( "Node_Game" ) );

        m_gameNode = node;

        m_scene->addChild( node );

        const Resolution & resolution = APPLICATION_SERVICE()
            ->getContentResolution();

        m_sceneSize.x = resolution.getWidthF();
        m_sceneSize.y = resolution.getHeightF();

        for( int32_t row = 0; row != GRID_SIZE_Y; row++ )
        {
            VectorInt vectorRow;

            m_map.push_back( vectorRow );
            for( int32_t col = 0; col != GRID_SIZE_X; col++ )
            {
                int32_t index = row * GRID_SIZE_X + col;

                m_map[row].push_back( index );
            }
        }

        m_semaphoreGameOver = GOAP_SERVICE()
            ->makeSemaphore( 0 );

        // setup background
        this->setupRandomMap();
        this->setupHotspots();

        bool setupBackgroundResult = this->setupBackground( _imagePath );

        MENGINE_ASSERTION_FATAL( setupBackgroundResult == true );

        return setupBackgroundResult;
    }
    //////////////////////////////////////////////////////////////////////////
    void FifteenSceneEventReceiver::createSplitBackgroundSprites( const ConstString & _name, const ConstString & _fileGroup, const FilePath & _filePath )
    {
        VectorShapeQuadFixedPtr backgroundParts;

        mt::vec2f splitSize( {1.f / GRID_SIZE_Y, 1.f / GRID_SIZE_X} );

        for( int32_t row = 0; row != GRID_SIZE_Y; row++ )
        {
            for( int32_t col = 0; col != GRID_SIZE_X; col++ )
            {
                ConstString name = Helper::stringizeStringFormat( "%s_%f_%f", _name.c_str(), row, col );

                mt::vec2f spriteShiftMin = {splitSize.x * col, splitSize.y * row};

                mt::vec2f spriteShiftMax = {spriteShiftMin.x + splitSize.x, spriteShiftMin.y + splitSize.y};
                ResourceImageDefaultPtr substractResource = this->createImageSubstractResource( name, _fileGroup, _filePath,
                    {-1.f, -1.f}, {spriteShiftMin, spriteShiftMax} );

                MENGINE_ASSERTION_MEMORY_PANIC( substractResource );

                ShapeQuadFixedPtr backgroundPart = this->createSprite( name, substractResource );

                MENGINE_ASSERTION_MEMORY_PANIC( backgroundPart );

                m_sprites.push_back( backgroundPart );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool FifteenSceneEventReceiver::setupBackground( const ConstString & _imagePath )
    {
        mt::vec2f hotspotSize( {m_sceneSize.x / GRID_SIZE_X, m_sceneSize.y / GRID_SIZE_Y} );

        m_minimumDistanceX = hotspotSize.x;
        m_minimumDistanceY = hotspotSize.y;

        FilePath imagePath = Helper::stringizeFilePath( _imagePath.c_str() );

        ResourceImageDefaultPtr resourceImage = this->createImageResource( STRINGIZE_STRING_LOCAL( "Background" ),
            STRINGIZE_STRING_LOCAL( "Assets" ), imagePath, {-1.f, -1.f} );

        if( resourceImage == nullptr )
        {
            return false;
        }

        m_backgroundSize = resourceImage->getMaxSize();

        m_background = this->createSprite( STRINGIZE_STRING_LOCAL( "BG" ), resourceImage );

        MENGINE_ASSERTION_MEMORY_PANIC( m_background );

        this->createSplitBackgroundSprites( STRINGIZE_STRING_LOCAL( "Background_Sprites" ), STRINGIZE_STRING_LOCAL( "Assets" ), imagePath );

        this->setupSprites();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FifteenSceneEventReceiver::runTaskChains()
    {
        GOAP::SourceInterfacePtr source = GOAP_SERVICE()
            ->makeSource();

        Cook::addWhile( source, [this]( const GOAP::SourceInterfacePtr & _scope_while )
        {
            auto && [race_play, race_end] = Cook::addRace<2>( _scope_while );

            Cook::addPrint( race_play, "NEW TURN" );
            Cook::addScope( race_play, this, &FifteenSceneEventReceiver::calculateAroundEmpty );
            Cook::addFunction( race_play, this, &FifteenSceneEventReceiver::checkWin );

            Cook::addSemaphoreEqual( race_end, m_semaphoreGameOver, 1 );
            Cook::addScope( race_end, this, &FifteenSceneEventReceiver::scopeGameOver );
            Cook::addFunction( race_end, this, &FifteenSceneEventReceiver::resetGame );

            return true;
        } );

        m_chain = GOAP_SERVICE()
            ->makeChain( source, nullptr, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE );

        m_chain->run();
    }
    //////////////////////////////////////////////////////////////////////////
    void FifteenSceneEventReceiver::makeTurn( const GOAP::SourceInterfacePtr & _scope, const HotSpotPolygonPtr & _hotspot )
    {
        HotSpotPolygonPtr empty = m_hotspots[m_emptyIndexY][m_emptyIndexX];

        Cook::addFunction( _scope, this, &FifteenSceneEventReceiver::swapPositions, empty, _hotspot );
    }
    //////////////////////////////////////////////////////////////////////////
    void FifteenSceneEventReceiver::resetGame()
    {
        uint32_t currentImage = m_imageIndex % IMAGE_COUNT;
        ConstString currentImageName = Helper::stringizeStringFormat( "image_%d.jpg", currentImage );

        this->clearSprites();
        this->clearHotspots();
        this->clearBackground();
        this->clearTaskChain();
        this->clearGameNode();
        this->clearMap();

        this->setupGame( currentImageName );

        m_imageIndex++;

        this->runTaskChains();
    }
    //////////////////////////////////////////////////////////////////////////
    void FifteenSceneEventReceiver::swapPositions( const HotSpotPolygonPtr & _empty, const HotSpotPolygonPtr & _hotspot )
    {
        const TransformationInterface * hotspotTransformation = _hotspot->getTransformation();

        int32_t newEmptyIndexX = static_cast<int32_t>(hotspotTransformation->getLocalPositionX() / m_minimumDistanceX);
        int32_t newEmptyIndexY = static_cast<int32_t>(hotspotTransformation->getLocalPositionY() / m_minimumDistanceY);
        int32_t newValue = m_map[newEmptyIndexY][newEmptyIndexX];
        int32_t oldValue = m_map[m_emptyIndexY][m_emptyIndexX];

        const TransformationInterface * emptyTransformation = _empty->getTransformation();

        mt::vec3f emptyPosition = emptyTransformation->getLocalPosition();
        mt::vec3f hotspotPosition = hotspotTransformation->getLocalPosition();

        m_hotspots[m_emptyIndexY][m_emptyIndexX].swap( m_hotspots[newEmptyIndexY][newEmptyIndexX] );
        
        TransformationInterface * oldTransformation = m_hotspots[m_emptyIndexY][m_emptyIndexX]->getTransformation();
        TransformationInterface * newTransformation = m_hotspots[newEmptyIndexY][newEmptyIndexX]->getTransformation();

        oldTransformation->setLocalPosition( emptyPosition );
        newTransformation->setLocalPosition( hotspotPosition );

        m_map[newEmptyIndexY][newEmptyIndexX] = oldValue;
        m_map[m_emptyIndexY][m_emptyIndexX] = newValue;

        m_emptyIndexX = newEmptyIndexX;
        m_emptyIndexY = newEmptyIndexY;
    }
    //////////////////////////////////////////////////////////////////////////
    void FifteenSceneEventReceiver::calculateAroundEmpty( const GOAP::SourceInterfacePtr & _scope )
    {
        m_hotspotsAroundEmpty.clear();

        if( m_emptyIndexY - 1 >= 0 )
        {
            m_hotspotsAroundEmpty.push_back( m_hotspots[m_emptyIndexY - 1][m_emptyIndexX] );
        }

        if( m_emptyIndexY + 1 < GRID_SIZE_Y )
        {
            m_hotspotsAroundEmpty.push_back( m_hotspots[m_emptyIndexY + 1][m_emptyIndexX] );
        }

        if( m_emptyIndexX - 1 >= 0 )
        {
            m_hotspotsAroundEmpty.push_back( m_hotspots[m_emptyIndexY][m_emptyIndexX - 1] );
        }

        if( m_emptyIndexX + 1 < GRID_SIZE_X )
        {
            m_hotspotsAroundEmpty.push_back( m_hotspots[m_emptyIndexY][m_emptyIndexX + 1] );
        }

        for( auto && [race, hotspot] : Cook::addRaceZip( _scope, m_hotspotsAroundEmpty ) )
        {
            Cook::addPickerableMouseButton( race, hotspot, EMouseCode::MC_LBUTTON, true, true, nullptr, MENGINE_DOCUMENT_FACTORABLE );

            Cook::addScope( race, this, &FifteenSceneEventReceiver::makeTurn, hotspot );
        }
    }

    //////////////////////////////////////////////////////////////////////////
    int32_t FifteenSceneEventReceiver::checkBiggerGroups( int32_t _row, int32_t _col )
    {
        int32_t currSum = 0;
        int32_t firstEnter = -1;
        int32_t currentNumber = m_map[_row][_col];

        for( int32_t row = _row; row != GRID_SIZE_Y; row++ )
        {
            if( firstEnter == -1 )
            {
                firstEnter = _col + 1;
            }
            else
            {
                firstEnter = 0;
            }

            for( int32_t col = firstEnter; col != GRID_SIZE_X; col++ )
            {
                int32_t nextNumber = m_map[row][col];

                if( currentNumber > nextNumber )
                {
                    currSum++;
                }
            }
        }

        return currSum;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FifteenSceneEventReceiver::checkSolution()
    {
        bool solutionExist = false;
        int32_t sum = 0;

        for( int32_t row = 0; row != GRID_SIZE_Y; row++ )
        {
            for( int32_t col = 0; col != GRID_SIZE_X; col++ )
            {
                if( row != m_emptyIndexY || col != m_emptyIndexX )
                {
                    sum += this->checkBiggerGroups( row, col );
                }
            }
        }

        sum += m_emptyIndexY + 1;
        if( sum % 2 == 0 )
        {
            solutionExist = true;
        }

        return solutionExist;
    }
    //////////////////////////////////////////////////////////////////////////
    void FifteenSceneEventReceiver::checkWin()
    {
        for( int32_t row = 0; row != GRID_SIZE_Y; row++ )
        {
            for( int32_t col = 0; col != GRID_SIZE_X; col++ )
            {
                int32_t hotspotId = m_map[row][col];
                int32_t currId = row * GRID_SIZE_Y + col;

                if( hotspotId != currId )
                {
                    return;
                }
            }
        }

        m_semaphoreGameOver->setValue( 1 );
    }
    //////////////////////////////////////////////////////////////////////////
    void FifteenSceneEventReceiver::enableSprites( bool _value )
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
    void FifteenSceneEventReceiver::enableBackground( bool _value )
    {
        if( _value == true )
        {
            m_background->enable();
        }
        else
        {
            m_background->disable();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool FifteenSceneEventReceiver::scopeGameOver( const GOAP::SourceInterfacePtr & _scope )
    {
        float delay = HIGHLIGHT_INTERVAL;
        int32_t count = HIGHLIGHT_COUNT;

        m_gameNode->addChild( m_background );

        this->clearSprites();

        Cook::addFor( _scope, count, [this, delay]( const GOAP::SourceInterfacePtr & _scope_for, uint32_t _iterator, uint32_t _count )
        {
            MENGINE_UNUSED( _iterator );

            Cook::addLocalDelay( _scope_for, delay, MENGINE_DOCUMENT_FACTORABLE );
            Cook::addFunction( _scope_for, this, &FifteenSceneEventReceiver::enableBackground, false );
            Cook::addLocalDelay( _scope_for, delay, MENGINE_DOCUMENT_FACTORABLE );
            Cook::addFunction( _scope_for, this, &FifteenSceneEventReceiver::enableBackground, true );

            if( _count == HIGHLIGHT_COUNT )
            {
                Cook::addFunction( _scope_for, m_semaphoreGameOver, &GOAP::SemaphoreInterface::setValue, 0 );
            }

            return true;
        } );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FifteenSceneEventReceiver::clearGameNode()
    {
        if( m_gameNode != nullptr )
        {
            m_gameNode->removeFromParent();
            m_gameNode = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void FifteenSceneEventReceiver::clearBackground()
    {
        if( m_background != nullptr )
        {
            m_background->removeFromParent();
            m_background = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void FifteenSceneEventReceiver::clearSprites()
    {
        for( ShapeQuadFixedPtr & sprite : m_sprites )
        {
            sprite->removeFromParent();
            sprite = nullptr;
        }

        m_sprites.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void FifteenSceneEventReceiver::clearMap()
    {
        for( VectorInt & row : m_map )
        {
            row.clear();
        }

        m_map.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void FifteenSceneEventReceiver::clearHotspots()
    {
        for( Vector<HotSpotPolygonPtr> & hotspots : m_hotspots )
        {
            for( HotSpotPolygonPtr & hotspot : hotspots )
            {
                hotspot->removeFromParent();
                hotspot = nullptr;
            }

            hotspots.clear();
        }

        m_hotspots.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void FifteenSceneEventReceiver::clearTaskChain()
    {
        if( m_chain != nullptr )
        {
            m_chain->cancel();
            m_chain = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
};