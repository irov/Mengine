#include "PuzzleSceneEventReceiver.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/CodecServiceInterface.h"

#include "Plugins/GOAPPlugin/Tasks/GOAPCook.h"
#include "Plugins/GraphicsPlugin/GraphicsInterface.h"

#include "Engine/Engine.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/Surface.h"
#include "Kernel/SurfaceSolidColor.h"
#include "Kernel/ShapeQuadFixed.h"
#include "Kernel/SurfaceImage.h"
#include "Kernel/ResourceImageDefault.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/StringHelper.h"
#include "Kernel/EntityHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/SchedulerHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/Vector.h"
#include "Kernel/Color.h"
#include "Kernel/Stringalized.h"

#include "math/uv4_inline.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static MENGINE_CONSTEXPR const int32_t PUZZLE_ROWS = 4;
    static MENGINE_CONSTEXPR const int32_t PUZZLE_COLS = 4;
    static MENGINE_CONSTEXPR const int32_t EMPTY_IDX = PUZZLE_ROWS * PUZZLE_COLS - 1;
    static MENGINE_CONSTEXPR const int32_t DIRECTION_UP = -PUZZLE_COLS;
    static MENGINE_CONSTEXPR const int32_t DIRECTION_DOWN = PUZZLE_COLS;
    static MENGINE_CONSTEXPR const int32_t DIRECTION_LEFT = -1;
    static MENGINE_CONSTEXPR const int32_t DIRECTION_RIGHT = 1;
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static MENGINE_INLINE const FilePath & IMAGE_PATH()
        {
            return STRINGIZE_FILEPATH_LOCAL( "image_0.png" );
        }
        //////////////////////////////////////////////////////////////////////////
        static MENGINE_INLINE void convertRowColToIdx( int32_t _row, int32_t _col, int32_t * _idx )
        {
            *_idx = PUZZLE_COLS * _row + _col;
        }
        //////////////////////////////////////////////////////////////////////////
        static MENGINE_INLINE void convertIdxToRowCol( int32_t _idx, int32_t * _row, int32_t * _col)
        {
            *_row = _idx / PUZZLE_COLS;
            *_col = _idx % PUZZLE_COLS;
        }
        //////////////////////////////////////////////////////////////////////////
        static void randomizeVectorIndexes( VectorIndexes * _vector, uint32_t _size, int32_t _min, int32_t _max )
        {
            const RandomizerInterfacePtr & randomizer = PLAYER_SERVICE()
                ->getRandomizer();

            _vector->clear();

            while( _vector->size() < _size )
            {
                int32_t value = randomizer->getRandomRangei( _min, _max );

                if( std::find( _vector->begin(), _vector->end(), value ) != _vector->end() )
                {
                    continue;
                }

                _vector->emplace_back( value );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static bool checkVectorIndexes( const VectorIndexes & _vector )
        {
            uint32_t sum = 0;
            uint32_t emptyIdx = 0;

            for( uint32_t i = 0; i != _vector.size() - 1; i++ )
            {
                if( _vector[i] == EMPTY_IDX )
                {
                    int32_t row;
                    int32_t col;
                    Detail::convertIdxToRowCol( i, &row, &col );
                    emptyIdx = row + 1;

                    continue;
                }

                for( uint32_t j = i + 1; j != _vector.size(); j++ )
                {
                    if( _vector[j] < _vector[i] )
                    {
                        sum++;
                    }
                }
            }

            sum += emptyIdx;

            LOGGER_WARNING( "CHECK SUM = %d"
                , sum
            );

            if( sum % 2 == 1 )
            {
                return false;
            }

            return true;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    PuzzleSceneEventReceiver::PuzzleSceneEventReceiver()
        : m_scene( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PuzzleSceneEventReceiver::~PuzzleSceneEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PuzzleSceneEventReceiver::onSceneAppMouseLeave( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onSceneAppMouseLeave [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PuzzleSceneEventReceiver::onSceneAppMouseEnter( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onSceneAppMouseEnter [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PuzzleSceneEventReceiver::onSceneAppFocus( const EntityBehaviorInterfacePtr & _behavior, bool _focus )
    {
        MENGINE_UNUSED( _behavior );
        MENGINE_UNUSED( _focus );

        LOGGER_MESSAGE( "Scene onSceneAppFocus [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PuzzleSceneEventReceiver::onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity )
    {
        MENGINE_UNUSED( _behavior );

        m_scene = Helper::staticNodeCast<Scene *>( _entity );

        LOGGER_MESSAGE( "Scene onEntityCreate [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PuzzleSceneEventReceiver::onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityDestroy [%s]"
            , m_scene->getName().c_str()
        );

        m_scene = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PuzzleSceneEventReceiver::onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityActivate [%s]"
            , m_scene->getName().c_str()
        );

        this->setupGame();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PuzzleSceneEventReceiver::onEntityActivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityActivate [%s]"
            , m_scene->getName().c_str()
        );

        this->runTaskChains();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PuzzleSceneEventReceiver::onEntityPreparationDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityPreparationDeactivate [%s]"
            , m_scene->getName().c_str()
        );

        this->clearTaskChain();
    }
    //////////////////////////////////////////////////////////////////////////
    void PuzzleSceneEventReceiver::onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );
        
        LOGGER_MESSAGE( "Scene onEntityDeactivate [%s]"
            , m_scene->getName().c_str()
        );

        this->clearSprites();
        this->clearHotspots();
        this->clearTexts();
        this->clearNodes();

        this->clearGameNode();
        this->clearGame();

        for( const ResourcePtr & resource : m_resources )
        {
            resource->finalize();
        }

        m_resources.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void PuzzleSceneEventReceiver::onEntityCompile( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityCompile [%s]"
            , m_scene->getName().c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    void PuzzleSceneEventReceiver::onEntityRelease( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityRelease [%s]"
            , m_scene->getName().c_str()
        );
    }
    /////////////////////////////////////////////////////////////////////////
    ShapeQuadFixedPtr PuzzleSceneEventReceiver::createSprite( const ConstString & _name, const ResourcePtr & _resource )
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
    HotSpotPolygonPtr PuzzleSceneEventReceiver::createHotSpot( const ConstString & _name, const mt::vec2f & _size )
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

        return hotspot;
    }
    /////////////////////////////////////////////////////////////////////////
    NodePtr PuzzleSceneEventReceiver::createGraphicsRect( const ConstString & _name, const Color & _color, const mt::vec2f & _size )
    {
        NodePtr graphicsNode = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Graphics" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( graphicsNode );

        graphicsNode->setName( _name );

        GraphicsInterface * graphics = graphicsNode->getUnknown();

        graphics->setLineColor( _color );

        graphics->drawRect( mt::vec2f{ 0.f, 0.f }, _size.x, _size.y );

        return graphicsNode;
    }
    /////////////////////////////////////////////////////////////////////////
    NodePtr PuzzleSceneEventReceiver::createGraphicsRectFill( const ConstString & _name, const Color & _color, const mt::vec2f & _size )
    {
        NodePtr graphicsNode = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Graphics" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( graphicsNode );

        graphicsNode->setName( _name );

        GraphicsInterface * graphics = graphicsNode->getUnknown();

        graphics->setLineColor( _color );
        
        graphics->beginFill();

        graphics->drawRect( mt::vec2f{ 0.f, 0.f }, _size.x, _size.y );

        graphics->endFill();

        return graphicsNode;
    }
    /////////////////////////////////////////////////////////////////////////
    TextFieldPtr PuzzleSceneEventReceiver::createTextField( const ConstString & _name, const ConstString & _id, const VectorString & _args )
    {
        TextFieldPtr text = Helper::generateTextField( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( text );

        text->setName( _name );

        text->setTextId( _id );

        if( _args.empty() == false )
        {
            text->setTextFormatArgs( _args );
        }

        return text;
    }
    /////////////////////////////////////////////////////////////////////////
    bool PuzzleSceneEventReceiver::getImageSize( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, mt::vec2f * _out ) const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _out );
        
        InputStreamInterfacePtr stream = Helper::openInputStreamFile( _fileGroup, _filePath, false, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream );

        const ConstString & codecType = CODEC_SERVICE()
            ->findCodecType( _filePath );

        ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE()
            ->createDecoderT<ImageDecoderInterfacePtr>( codecType, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( imageDecoder );

        if( imageDecoder->prepareData( stream ) == false )
        {
            return false;
        }

        const ImageCodecDataInfo * dataInfo = imageDecoder->getCodecDataInfo();

        _out->x = (float)dataInfo->width;
        _out->y = (float)dataInfo->height;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceImageDefaultPtr PuzzleSceneEventReceiver::createImageResource( const ConstString & _resourceName, const ConstString & _fileGroupName, const FilePath & _filePath, const mt::vec2f & _maxSize, const mt::uv4f & _uvImage, const mt::uv4f & _uvAlpha )
    {
        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getFileGroup( _fileGroupName );

        mt::vec2f maxSize;

        if( _maxSize.x < 0.f || _maxSize.y < 0.f )
        {
            if( this->getImageSize( fileGroup, _filePath, &maxSize ) == false )
            {
                return nullptr;
            }
        }
        else
        {
            maxSize = _maxSize;
        }

        ResourceImageDefaultPtr resource = PROTOTYPE_GENERATE( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceImageDefault" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( resource );

        resource->setName( _resourceName );

        ContentInterfacePtr content = PROTOTYPE_GENERATE( STRINGIZE_STRING_LOCAL( "FileContent" ), ConstString::none(), MENGINE_DOCUMENT_FACTORABLE );

        content->setFileGroup( fileGroup );
        content->setFilePath( _filePath );

        const ConstString & newCodecType = CODEC_SERVICE()
            ->findCodecType( _filePath );

        if( newCodecType.empty() == true )
        {
            return nullptr;
        }

        content->setCodecType( newCodecType );

        resource->setContent( content );

        resource->setMaxSize( maxSize );
        resource->setSize( maxSize );
        resource->setOffset( mt::vec2f( 0.f, 0.f ) );

        resource->setUVImage( _uvImage );
        resource->setUVAlpha( _uvAlpha );

        resource->setAlpha( true );

        if( resource->initialize() == false )
        {
            LOGGER_ERROR( "invalid initialize image '%s:%s'"
                , fileGroup->getName().c_str()
                , _filePath.c_str()
            );

            return nullptr;
        }

        m_resources.push_back( resource );

        return resource;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PuzzleSceneEventReceiver::setupGame()
    {
        FilePath imageFilePath = Detail::IMAGE_PATH();

        // create game node
        NodePtr node = PROTOTYPE_GENERATE( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Interender" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( node );

        node->setName( STRINGIZE_STRING_LOCAL( "Node_Game" ) );

        m_gameNode = node;

        m_scene->addChild( node );

        // semaphores
        m_semaphoreGameOver = GOAP_SERVICE()
            ->makeSemaphore( 0 );

        m_semaphoreRandomOver = GOAP_SERVICE()
            ->makeSemaphore( 0 );

        // full image size
        mt::vec2f imageSize;

        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getFileGroup( STRINGIZE_STRING_LOCAL( "Assets" ) );

        this->getImageSize( fileGroup, imageFilePath, &imageSize );

        // sprites
        mt::vec2f imagePartSize( imageSize.x / PUZZLE_COLS, imageSize.y / PUZZLE_ROWS);

        mt::vec2f uv_size(
            imagePartSize.x / imageSize.x
            , imagePartSize.y / imageSize.y
        );

        Color colorLine( Helper::makeRGBA8( 0, 255, 0, 255 ) );
        Color colorFill( Helper::makeRGBA8( 0, 0, 255, 50 ) );

        for( int row = 0; row != PUZZLE_ROWS; row++ )
        {
            for( int col = 0; col != PUZZLE_COLS; col++ )
            {
                mt::vec3f position(
                    imagePartSize.x * col
                    , imagePartSize.y * row
                    , 0.f
                );

                m_positions.emplace_back( position );

                int32_t idx;

                Detail::convertRowColToIdx( row, col, &idx );

                int32_t row_test;
                int32_t col_test;

                Detail::convertIdxToRowCol( idx, &row_test, &col_test );

                MENGINE_ASSERTION( row == row_test || col == col_test );

                m_indexes.emplace_back( idx );

                
            }
        }

        for( int row = 0; row != PUZZLE_ROWS; row++ )
        {
            for( int col = 0; col != PUZZLE_COLS; col++ )
            {
                int32_t idx;

                Detail::convertRowColToIdx( row, col, &idx );

                mt::vec3f position = m_positions[idx];

                // sprite resource
                mt::vec2f uv_pos( 
                    position.x / imageSize.x
                    , position.y / imageSize.y
                );

                mt::vec4f uv_mask( 
                    uv_pos.x
                    , uv_pos.y
                    , uv_pos.x + uv_size.x
                    , uv_pos.y + uv_size.y
                );

                mt::uv4f uv;

                mt::uv4_from_mask( uv, uv_mask );

                ResourceImageDefaultPtr resourceImage = this->createImageResource(
                    STRINGIZE_STRING_LOCAL( "image" )
                    , STRINGIZE_STRING_LOCAL( "Assets" )
                    , imageFilePath
                    , imagePartSize
                    , uv
                    , uv
                );

                // sprite node
                ShapeQuadFixedPtr sprite = this->createSprite( STRINGIZE_STRING_LOCAL( "image" ), resourceImage );

                sprite->setName( Helper::stringizeStringFormat( "Sprite_r%d_c%d", row, col ) );

                TransformationInterface * transformable = sprite->getTransformation();
                transformable->setLocalPosition( position );

                m_gameNode->addChild( sprite );

                m_sprites.emplace_back( sprite );

                m_parts.emplace_back( sprite );

                // text
                if ( idx != EMPTY_IDX )
                {
                    VectorString text_args;

                    Char str[32];

                    Helper::stringalized( idx + 1, str, 32 );

                    text_args.emplace_back( str );

                    mt::vec3f textPosition( imagePartSize.x / 2, imagePartSize.y / 2, 0.f );

                    float borderOffset = 2.f;

                    // up border
                    TextFieldPtr textUpBorder = this->createTextField(
                        Helper::stringizeStringFormat( "TextField_r%d_c%d_Value_Border_Up", row, col )
                        , STRINGIZE_STRING_LOCAL( "ID_Value_Border" )
                        , text_args
                    );

                    TransformationInterface * transformationTextUpBorder = textUpBorder->getTransformation();
                    transformationTextUpBorder->setLocalPosition( textPosition + mt::vec3f( 0.f, -borderOffset, 0.f ) );

                    sprite->addChild( textUpBorder );

                    m_texts.emplace_back( textUpBorder );

                    // bottom border
                    TextFieldPtr textBottomBorder = this->createTextField(
                        Helper::stringizeStringFormat( "TextField_r%d_c%d_Value_Border_Bottom", row, col )
                        , STRINGIZE_STRING_LOCAL( "ID_Value_Border" )
                        , text_args
                    );

                    TransformationInterface * transformationTextBottomBorder = textBottomBorder->getTransformation();
                    transformationTextBottomBorder->setLocalPosition( textPosition + mt::vec3f( 0.f, borderOffset, 0.f ) );

                    sprite->addChild( textBottomBorder );

                    m_texts.emplace_back( textBottomBorder );

                    // left border
                    TextFieldPtr textLeftBorder = this->createTextField(
                        Helper::stringizeStringFormat( "TextField_r%d_c%d_Value_Border_Left", row, col )
                        , STRINGIZE_STRING_LOCAL( "ID_Value_Border" )
                        , text_args
                    );

                    TransformationInterface * transformationTextLeftBorder = textLeftBorder->getTransformation();
                    transformationTextLeftBorder->setLocalPosition( textPosition + mt::vec3f( -borderOffset, 0.f, 0.f ) );

                    sprite->addChild( textLeftBorder );

                    m_texts.emplace_back( textLeftBorder );

                    // right border
                    TextFieldPtr textRightBorder = this->createTextField(
                        Helper::stringizeStringFormat( "TextField_r%d_c%d_Value_Border_Right", row, col )
                        , STRINGIZE_STRING_LOCAL( "ID_Value_Border" )
                        , text_args
                    );

                    TransformationInterface * transformationTextRightBorder = textRightBorder->getTransformation();
                    transformationTextRightBorder->setLocalPosition( textPosition + mt::vec3f( borderOffset, 0.f, 0.f ) );

                    sprite->addChild( textRightBorder );

                    m_texts.emplace_back( textRightBorder );

                    // value
                    TextFieldPtr textValue = this->createTextField(
                        Helper::stringizeStringFormat( "TextField_r%d_c%d_Value", row, col )
                        , STRINGIZE_STRING_LOCAL( "ID_Value" )
                        , text_args
                    );

                    TransformationInterface * transformationTextValue = textValue->getTransformation();
                    transformationTextValue->setLocalPosition( textPosition );

                    sprite->addChild( textValue );

                    m_texts.emplace_back( textValue );
                }

                
            }
        }

        // add to last part half transpared rect
        NodePtr lastPart = m_parts.back();

        NodePtr graphicsLastPart = createGraphicsRectFill( STRINGIZE_STRING_LOCAL( "Graphics_Last_Part_Fill" ), colorFill, imagePartSize );

        lastPart->addChild( graphicsLastPart );

        m_nodes.emplace_back( graphicsLastPart );

        // border
        NodePtr graphics = createGraphicsRect( STRINGIZE_STRING_LOCAL( "border" ), colorLine, imageSize );

        m_gameNode->addChild( graphics );

        m_nodes.emplace_back( graphics );

        for( uint32_t idx = 0; idx != m_positions.size(); idx++ )
        {
            const mt::vec3f & position = m_positions[idx];

            int32_t row;
            int32_t col;
            Detail::convertIdxToRowCol( idx, &row, &col );

            // border
            NodePtr graphicsCell = createGraphicsRect( Helper::stringizeStringFormat( "Graphics_%d_%d_Cell", row, col ), colorLine, imagePartSize );

            TransformationInterface * transformationGraphicsCell = graphicsCell->getTransformation();
            transformationGraphicsCell->setLocalPosition( position );

            m_gameNode->addChild( graphicsCell );

            m_nodes.emplace_back( graphicsCell );

            // hotspot
            HotSpotPolygonPtr hotspot = this->createHotSpot( Helper::stringizeStringFormat( "HotSpot_%d_%d", row, col ), imagePartSize );

            hotspot->setName( Helper::stringizeStringFormat( "HotSpot_r%d_c%d", row, col ) );

            m_gameNode->addChild( hotspot );

            TransformationInterface * transformationHotspot = hotspot->getTransformation();
            transformationHotspot->setLocalPosition( position );

            m_hotspots.emplace_back( hotspot );
        }

        // setup game node position
        const Resolution & resolution = APPLICATION_SERVICE()
            ->getContentResolution();

        float resolutionWidth = resolution.getWidthF();
        float resolutionHeight = resolution.getHeightF();

        TransformationInterface * transformationGameNode = m_gameNode->getTransformation();
        transformationGameNode->setLocalPosition( { (resolutionWidth - imageSize.x) / 2, (resolutionHeight - imageSize.y) / 2, 0.f } );

        // disable borders
        this->enableBorders( false );
        this->enableTexts( false );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PuzzleSceneEventReceiver::enableBorders( bool _value )
    {
        for( const NodePtr & node : m_nodes )
        {
            if( _value == true )
            {
                node->enable();
            }
            else
            {
                node->disable();
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void PuzzleSceneEventReceiver::enableTexts( bool _value )
    {
        for( const TextFieldPtr & text : m_texts )
        {
            if( _value == true )
            {
                text->enable();
            }
            else
            {
                text->disable();
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void PuzzleSceneEventReceiver::runTaskChains()
    {
        GOAP::SourceInterfacePtr source = GOAP_SERVICE()
            ->makeSource();

        Cook::addWhile( source, [this]( const GOAP::SourceInterfacePtr & _scope_while )
        {
            Cook::addPrint( _scope_while, "NEW GAME" );

            // wait start
            auto && [race_btn_space, race_mouse_l] = Cook::addRace<2>( _scope_while );

            Cook::addGlobalKeyPress( race_btn_space, EKeyCode::KC_SPACE, true, nullptr, MENGINE_DOCUMENT_FACTORABLE );
            Cook::addGlobalMouseButton( race_mouse_l, EMouseCode::MC_LBUTTON, true, nullptr, MENGINE_DOCUMENT_FACTORABLE );

            // start
            Cook::addFunction( _scope_while, this, &PuzzleSceneEventReceiver::enableBorders, true );

            Cook::addGlobalDelay( _scope_while, 500.f, MENGINE_DOCUMENT_FACTORABLE );

            Cook::addFunction( _scope_while, this, &PuzzleSceneEventReceiver::enableTexts, true );

            Cook::addSemaphoreAssign( _scope_while, m_semaphoreRandomOver, 0 );

            GOAP::SourceInterfacePtr scope_until_random = Cook::addRepeat( _scope_while, [this]( const GOAP::SourceInterfacePtr & _scope_repeat )
            {
                Cook::addGlobalDelay( _scope_repeat, 150.f, MENGINE_DOCUMENT_FACTORABLE );

                Cook::addFunction( _scope_repeat, [this]()
                {
                    Detail::randomizeVectorIndexes( &m_indexes, PUZZLE_COLS * PUZZLE_ROWS, 0, PUZZLE_COLS * PUZZLE_ROWS );

                    for( uint32_t idx = 0; idx != (uint32_t)m_indexes.size(); idx++ )
                    {
                        const int32_t curIdx = m_indexes[idx];
                        const mt::vec3f & position = m_positions[idx];
                        const ShapeQuadFixedPtr & part = m_parts[curIdx];

                        TransformationInterface * transformationPart = part->getTransformation();
                        transformationPart->setLocalPosition( position );
                    }
                } );

                auto && [source_true, source_false] = GOAP::Cook::addIf( _scope_repeat, [this]()
                {
                    return Detail::checkVectorIndexes( m_indexes ) == true && this->isGameOver() == false;
                } );

                Cook::addSemaphoreAssign( source_true, m_semaphoreRandomOver, 1 );

                return true;
            } );

            Cook::addSemaphoreEqual( scope_until_random, m_semaphoreRandomOver, 1 );

            auto && [race_reset, race_quit, race_play] = Cook::addRace<3>( _scope_while );

            // reset
            auto && [race_reset_btn_r, race_reset_btn_space] = Cook::addRace<2>( race_reset );

            Cook::addGlobalKeyPress( race_reset_btn_r, EKeyCode::KC_R, true, nullptr, MENGINE_DOCUMENT_FACTORABLE );
            Cook::addGlobalKeyPress( race_reset_btn_space, EKeyCode::KC_SPACE, true, nullptr, MENGINE_DOCUMENT_FACTORABLE );

            Cook::addPrint( race_reset, "RESET GAME" );

            // quit
            auto && [race_quit_btn_q, race_quit_btn_esc] = Cook::addRace<2>( race_quit );

            Cook::addGlobalKeyPress( race_quit_btn_q, EKeyCode::KC_Q, true, nullptr, MENGINE_DOCUMENT_FACTORABLE );
            Cook::addGlobalKeyPress( race_quit_btn_esc, EKeyCode::KC_ESCAPE, true, nullptr, MENGINE_DOCUMENT_FACTORABLE );

            Cook::addPrint( race_quit, "GOODBYE!" );

            Cook::addFunction( race_quit, []()
            {
                APPLICATION_SERVICE()
                    ->quit();
            } );

            // play
            GOAP::SourceInterfacePtr scope_until_play = Cook::addRepeat( race_play, [this]( const GOAP::SourceInterfacePtr & _scope_repeat )
            {
                auto && [race_turn_hotspot, race_turn_arrows, race_text] = Cook::addRace<3>( _scope_repeat );

                // turn hotspot
                uint32_t idx = 0;
                for( auto && [race, hotpspot] : Cook::addRaceZip( race_turn_hotspot, m_hotspots ) )
                {
                    Cook::addPickerableMouseButton( race, hotpspot, EMouseCode::MC_LBUTTON, true, true, nullptr, MENGINE_DOCUMENT_FACTORABLE );

                    Cook::addScope( race, this, &PuzzleSceneEventReceiver::scopeMakeTurn, idx );

                    ++idx;
                }

                // turn arrows
                auto && [race_turn_arrows_up, race_turn_arrows_down, race_turn_arrows_left, race_turn_arrows_right] = Cook::addRace<4>( race_turn_arrows );

                Cook::addGlobalKeyPress( race_turn_arrows_up, EKeyCode::KC_UP, true, nullptr, MENGINE_DOCUMENT_FACTORABLE );
                Cook::addScope( race_turn_arrows_up, this, &PuzzleSceneEventReceiver::scopeMakeTurnToDirection, DIRECTION_DOWN );

                Cook::addGlobalKeyPress( race_turn_arrows_down, EKeyCode::KC_DOWN, true, nullptr, MENGINE_DOCUMENT_FACTORABLE );
                Cook::addScope( race_turn_arrows_down, this, &PuzzleSceneEventReceiver::scopeMakeTurnToDirection, DIRECTION_UP );

                Cook::addGlobalKeyPress( race_turn_arrows_left, EKeyCode::KC_LEFT, true, nullptr, MENGINE_DOCUMENT_FACTORABLE );
                Cook::addScope( race_turn_arrows_left, this, &PuzzleSceneEventReceiver::scopeMakeTurnToDirection, DIRECTION_RIGHT );

                Cook::addGlobalKeyPress( race_turn_arrows_right, EKeyCode::KC_RIGHT, true, nullptr, MENGINE_DOCUMENT_FACTORABLE );
                Cook::addScope( race_turn_arrows_right, this, &PuzzleSceneEventReceiver::scopeMakeTurnToDirection, DIRECTION_LEFT );

                // text
                auto && [race_text_btn_t, race_text_mouse_r] = Cook::addRace<2>( race_text );

                Cook::addGlobalKeyPress( race_text_btn_t, EKeyCode::KC_T, true, nullptr, MENGINE_DOCUMENT_FACTORABLE );
                Cook::addGlobalMouseButton( race_text_mouse_r, EMouseCode::MC_RBUTTON, true, nullptr, MENGINE_DOCUMENT_FACTORABLE );

                Cook::addFunction( race_text, [this]()
                {
                    for( const TextFieldPtr & text : m_texts )
                    {
                        if( text->isEnable() == true )
                        {
                            text->disable();
                        }
                        else
                        {
                            text->enable();
                        }
                    }
                } );

                return true;
            } );

            Cook::addSemaphoreEqual( scope_until_play, m_semaphoreGameOver, 1 );
            
            Cook::addPrint( _scope_while, "GAME OVER" );

            Cook::addScope( _scope_while, this, &PuzzleSceneEventReceiver::scopeGameOver );
            Cook::addFunction( _scope_while, this, &PuzzleSceneEventReceiver::resetGame );

            return true;
        } );

        GOAP::ChainInterfacePtr chain = GOAP_SERVICE()
            ->makeChain( source, nullptr, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE );

        chain->run();

        m_chain = chain;
    }
    //////////////////////////////////////////////////////////////////////////
    void PuzzleSceneEventReceiver::scopeMakeTurn( const GOAP::SourceInterfacePtr & _scope, int32_t _position )
    {
        Cook::addPrint( _scope, "Click at idx '%d' hotspot", _position );

        // find index of empty cell
        int32_t emptyIdx = 0;
        for( const int32_t index : m_indexes )
        {
            if( index == EMPTY_IDX )
            {
                break;
            }

            emptyIdx++;
        }

        // check neighbour
        int32_t direction = emptyIdx - _position;

        bool isEmptyIdxNeigbour = direction == 1 || direction == -1 || direction == PUZZLE_COLS || direction == -PUZZLE_COLS;

        if( isEmptyIdxNeigbour == false )
        {
            return;
        }

        int32_t fromRow;
        int32_t fromCol;
        Detail::convertIdxToRowCol( emptyIdx, &fromRow, &fromCol );

        int32_t toRow;
        int32_t toCol;
        Detail::convertIdxToRowCol( _position, &toRow, &toCol );

        if( (direction == DIRECTION_LEFT || direction == DIRECTION_RIGHT) && (fromRow != toRow) )
        {
            return;
        }

        if( (direction == DIRECTION_UP || direction == DIRECTION_DOWN) && (fromCol != toCol) )
        {
            return;
        }

        // swap
        int32_t toIdx = m_indexes[_position];
        int32_t fromIdx = m_indexes[emptyIdx];

        NodePtr nodeAtToIdx = m_sprites[toIdx];
        NodePtr nodeAtFromIdx = m_sprites[fromIdx];

        mt::vec3f positionOfToIdx = m_positions[_position];
        mt::vec3f positionOfFromIdx = m_positions[emptyIdx];

        TransformationInterface * transformationNodeAtToIdx = nodeAtToIdx->getTransformation();
        transformationNodeAtToIdx->setLocalPosition( positionOfFromIdx );

        TransformationInterface * transformationNodeAtFromIdx = nodeAtFromIdx->getTransformation();
        transformationNodeAtFromIdx->setLocalPosition( positionOfToIdx );

        m_indexes[_position] = fromIdx;
        m_indexes[emptyIdx] = toIdx;

        if( this->isGameOver() == true )
        {
            m_semaphoreGameOver->setValue( 1 );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void PuzzleSceneEventReceiver::scopeMakeTurnToDirection( const GOAP::SourceInterfacePtr & _scope, int32_t _direction )
    {
        Cook::addPrint( _scope, "Click at _direction '%d'", _direction );

        // find index of empty cell
        int32_t fromIdx = 0;
        for( const int32_t index : m_indexes )
        {
            if( index == EMPTY_IDX )
            {
                break;
            }

            fromIdx++;
        }

        int32_t toIdx = fromIdx + _direction;

        if( toIdx < 0 || toIdx > EMPTY_IDX )
        {
            return;
        }

        int32_t fromRow;
        int32_t fromCol;
        Detail::convertIdxToRowCol( fromIdx, &fromRow, &fromCol );

        int32_t toRow;
        int32_t toCol;
        Detail::convertIdxToRowCol( toIdx, &toRow, &toCol );

        if( (_direction == DIRECTION_LEFT || _direction == DIRECTION_RIGHT) && (fromRow != toRow) )
        {
            return;
        }

        if( (_direction == DIRECTION_UP || _direction == DIRECTION_DOWN) && (fromCol != toCol) )
        {
            return;
        }

        Cook::addScope( _scope, this, &PuzzleSceneEventReceiver::scopeMakeTurn, toIdx );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PuzzleSceneEventReceiver::isGameOver() const
    {
        for( uint32_t idx = 0; idx != m_indexes.size() - 1; idx++ )
        {
            if( m_indexes[idx] > m_indexes[idx + 1] )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PuzzleSceneEventReceiver::scopeGameOver( const GOAP::SourceInterfacePtr & _scope )
    {
        Cook::addPrint( _scope, "SCOPE GAME OVER" );

        float delay = 100.f;
        int32_t count = 3;

        Cook::addFor( _scope, count, [this, delay]( const GOAP::SourceInterfacePtr & _scope_for, uint32_t _iterator, uint32_t _count )
        {
            MENGINE_UNUSED( _iterator );
            MENGINE_UNUSED( _count );

            Cook::addLocalDelay( _scope_for, delay, MENGINE_DOCUMENT_FACTORABLE );

            Cook::addFunction( _scope_for, this, &PuzzleSceneEventReceiver::enableBorders, false );

            Cook::addLocalDelay( _scope_for, delay, MENGINE_DOCUMENT_FACTORABLE );

            Cook::addFunction( _scope_for, this, &PuzzleSceneEventReceiver::enableBorders, true );

            return true;
        } );

        Cook::addLocalDelay( _scope, delay, MENGINE_DOCUMENT_FACTORABLE );

        Cook::addFunction( _scope, this, &PuzzleSceneEventReceiver::enableBorders, false );
        Cook::addFunction( _scope, this, &PuzzleSceneEventReceiver::enableTexts, false );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PuzzleSceneEventReceiver::resetGame()
    {
        m_semaphoreGameOver->setValue( 0 );
    }
    //////////////////////////////////////////////////////////////////////////
    void PuzzleSceneEventReceiver::clearGame()
    {
        m_semaphoreRandomOver = nullptr;
        m_semaphoreGameOver = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void PuzzleSceneEventReceiver::clearGameNode()
    {
        if( m_gameNode != nullptr )
        {
            m_gameNode->dispose();
            m_gameNode = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void PuzzleSceneEventReceiver::clearSprites()
    {
        for( const ShapeQuadFixedPtr & sprite : m_sprites )
        {
            sprite->dispose();
        }

        m_sprites.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void PuzzleSceneEventReceiver::clearHotspots()
    {
        for( const HotSpotPolygonPtr & hotspot : m_hotspots )
        {
            hotspot->dispose();
        }

        m_hotspots.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void PuzzleSceneEventReceiver::clearTexts()
    {
        for( const TextFieldPtr & text: m_texts )
        {
            text->dispose();
        }

        m_hotspots.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void PuzzleSceneEventReceiver::clearNodes()
    {
        for( const NodePtr & node: m_nodes)
        {
            node->dispose();
        }

        m_nodes.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void PuzzleSceneEventReceiver::clearTaskChain()
    {
        if( m_chain != nullptr )
        {
            m_chain->cancel();
            m_chain = nullptr;
        }
    }
};