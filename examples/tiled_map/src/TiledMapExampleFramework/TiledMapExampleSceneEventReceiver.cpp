#include "TiledMapExampleSceneEventReceiver.h"
#include "TiledMapExampleAutomappingHelper.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/RenderTextureServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Entity.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/NodeCast.h"
#include "Kernel/PathHelper.h"
#include "Kernel/VectorFilePath.h"

#include "Config/StdAlgorithm.h"
#include "Config/StdString.h"

#include "imgui.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static const Char * s_orientationName( ETiledMapOrientation _orientation )
        {
            switch( _orientation )
            {
            case ETMO_ORTHOGONAL:
                return "orthogonal";
            case ETMO_ISOMETRIC:
                return "isometric";
            case ETMO_HEXAGONAL:
                return "hexagonal";
            }

            return "unknown";
        }
    }
    //////////////////////////////////////////////////////////////////////////
    TiledMapExampleSceneEventReceiver::TiledMapExampleSceneEventReceiver()
        : m_scene( nullptr )
        , m_mapIndex( 0 )
        , m_excludedMapCount( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TiledMapExampleSceneEventReceiver::~TiledMapExampleSceneEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TiledMapExampleSceneEventReceiver::onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity )
    {
        MENGINE_UNUSED( _behavior );

        m_scene = Helper::staticNodeCast<Scene *>( _entity );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TiledMapExampleSceneEventReceiver::onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        m_scene = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TiledMapExampleSceneEventReceiver::onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        this->setupVirtualArea_();

        if( this->loadExamples_() == false )
        {
            return false;
        }

        if( this->loadMap_( 0 ) == false )
        {
            return false;
        }

        ImGUIRenderPtr imguiRender = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ImGUIRender" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( imguiRender, "invalid create ImGUIRender" );

        imguiRender->setName( STRINGIZE_STRING_LOCAL( "TiledMapExampleImGUI" ) );
        imguiRender->setProvider( [this]( const ImGUIRenderProviderInterfacePtr & _provider )
        {
            this->renderControls_( _provider );
        } );

        m_scene->addChild( imguiRender );
        m_imguiRender = imguiRender;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TiledMapExampleSceneEventReceiver::onEntityActivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        this->updateContentBounds_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TiledMapExampleSceneEventReceiver::onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        this->clearMap_();

        if( m_imguiRender != nullptr )
        {
            m_imguiRender->dispose();
            m_imguiRender = nullptr;
        }

        if( m_virtualArea != nullptr )
        {
            m_virtualArea->dispose();
            m_virtualArea = nullptr;
        }

        m_examples.clear();
        m_excludedMapCount = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    void TiledMapExampleSceneEventReceiver::setupVirtualArea_()
    {
        const Resolution & resolution = APPLICATION_SERVICE()
            ->getContentResolution();

        VirtualAreaPtr virtualArea = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "VirtualArea" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( virtualArea, "invalid create VirtualArea" );

        virtualArea->setName( STRINGIZE_STRING_LOCAL( "TiledMapVirtualArea" ) );
        virtualArea->setViewport( Viewport( 24.f, 96.f, resolution.getWidthF() - 24.f, resolution.getHeightF() - 24.f ) );
        virtualArea->setContentSize( 0.f, 0.f, resolution.getWidthF(), resolution.getHeightF() );
        virtualArea->setDraggingMode( EVADM_FREE );
        virtualArea->setDefaultHandle( true );
        virtualArea->setScaleEnable( true );
        virtualArea->setMaxScaleFactor( 8.f );
        virtualArea->setMaxScaleOutFactor( 2.f );
        virtualArea->setWheelScaleFactor( 0.1f );

        m_scene->addChild( virtualArea );
        m_virtualArea = virtualArea;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TiledMapExampleSceneEventReceiver::loadExamples_()
    {
        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getFileGroup( STRINGIZE_STRING_LOCAL( "TiledExamples" ) );

        if( fileGroup == nullptr )
        {
            m_error = "Can't find Tiled examples file group";

            return false;
        }

        VectorFilePath ruleListPaths;

        if( fileGroup->findFiles( FilePath::none(), "rules.txt", [&ruleListPaths]( const FilePath & _filePath )
            {
                String normalizedPath = _filePath.c_str();
                Helper::pathCorrectBackslashA( normalizedPath.data() );
                ruleListPaths.emplace_back( Helper::stringizeFilePath( normalizedPath ) );

                return true;
            } ) == false )
        {
            m_error = "Can't enumerate Tiled Automapping rule lists";

            return false;
        }

        VectorFilePath parsedRuleLists;
        VectorFilePath ruleMaps;

        for( const FilePath & ruleListPath : ruleListPaths )
        {
            if( Helper::collectTiledMapAutomappingRuleMaps( fileGroup, ruleListPath, &parsedRuleLists, &ruleMaps ) == false )
            {
                m_error = "Can't read a Tiled Automapping rule list";

                return false;
            }
        }

        VectorFilePath mapPaths;

        if( fileGroup->findFiles( FilePath::none(), "*.tmx", [&mapPaths]( const FilePath & _filePath )
            {
                String normalizedPath = _filePath.c_str();
                Helper::pathCorrectBackslashA( normalizedPath.data() );
                mapPaths.emplace_back( Helper::stringizeFilePath( normalizedPath ) );

                return true;
            } ) == false )
        {
            m_error = "Can't enumerate official Tiled maps";

            return false;
        }

        StdAlgorithm::sort( mapPaths.begin(), mapPaths.end(), []( const FilePath & _left, const FilePath & _right )
        {
            return StdString::strcmp( _left.c_str(), _right.c_str() ) < 0;
        } );

        for( const FilePath & mapPath : mapPaths )
        {
            VectorFilePath::const_iterator ruleMapIt = StdAlgorithm::find_if( ruleMaps.begin(), ruleMaps.end(), [&mapPath]( const FilePath & _filePath )
            {
                return StdString::strcmp( _filePath.c_str(), mapPath.c_str() ) == 0;
            } );

            if( ruleMapIt != ruleMaps.end() )
            {
                ++m_excludedMapCount;

                continue;
            }

            ResourceTiledMapPtr mapResource = this->createMapResource_( mapPath );

            if( mapResource == nullptr )
            {
                ++m_excludedMapCount;

                continue;
            }

            bool hasTileLayers = mapResource->getTileLayers().empty() == false;
            ETiledMapOrientation orientation = mapResource->getOrientation();

            mapResource->release();
            RESOURCE_SERVICE()
                ->removeResource( mapResource );
            m_resources.pop_back();

            if( hasTileLayers == false )
            {
                ++m_excludedMapCount;

                continue;
            }

            TiledMapExampleDesc desc;
            desc.title = mapPath.c_str();
            desc.title += " (";
            desc.title += Detail::s_orientationName( orientation );
            desc.title += ')';
            desc.mapPath = mapPath;

            m_examples.emplace_back( desc );
        }

        if( m_examples.empty() == true )
        {
            m_error = "No supported official Tiled maps found";

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TiledMapExampleSceneEventReceiver::loadMap_( uint32_t _index )
    {
        this->clearMap_();

        m_error.clear();
        m_mapIndex = _index % (uint32_t)m_examples.size();

        const TiledMapExampleDesc & desc = m_examples[m_mapIndex];
        ResourceTiledMapPtr mapResource = this->createMapResource_( desc.mapPath );

        if( mapResource == nullptr )
        {
            m_error = "Can't create Tiled map resource";
            this->clearMap_();

            return false;
        }

        VectorTiledMapExampleImageDescs imageDescs;

        const VectorTiledMapTilesets & tilesets = mapResource->getTilesets();

        for( const TiledMapTileset & tileset : tilesets )
        {
            if( tileset.resourceName.empty() == false )
            {
                TiledMapExampleImageDesc imageDesc;
                imageDesc.name = Helper::stringizeString( tileset.resourceName );
                imageDesc.path = tileset.imagePath;
                imageDescs.emplace_back( imageDesc );
            }

            for( const TiledMapTileImage & tileImage : tileset.tileImages )
            {
                TiledMapExampleImageDesc imageDesc;
                imageDesc.name = Helper::stringizeString( tileImage.resourceName );
                imageDesc.path = tileImage.imagePath;
                imageDescs.emplace_back( imageDesc );
            }
        }

        for( VectorTiledMapExampleImageDescs::const_iterator it = imageDescs.begin(); it != imageDescs.end(); ++it )
        {
            const TiledMapExampleImageDesc & imageDesc = *it;
            bool duplicateImage = false;

            for( VectorTiledMapExampleImageDescs::const_iterator duplicateIt = imageDescs.begin(); duplicateIt != it; ++duplicateIt )
            {
                if( duplicateIt->name != imageDesc.name )
                {
                    continue;
                }

                if( duplicateIt->path != imageDesc.path )
                {
                    m_error = "Tileset image resource name refers to different files";
                    this->clearMap_();

                    return false;
                }

                duplicateImage = true;

                break;
            }

            if( duplicateImage == true )
            {
                continue;
            }

            if( imageDesc.name.empty() == true || imageDesc.path.empty() == true
                || this->createImageResource_( imageDesc.name, imageDesc.path ) == nullptr )
            {
                m_error = "Can't create tileset image resource";
                this->clearMap_();

                return false;
            }
        }

        TiledMap2DPtr tiledMap = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "TiledMap2D" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( tiledMap, "invalid create TiledMap2D" );

        tiledMap->setName( Helper::stringizeString( desc.mapPath.c_str() ) );
        tiledMap->setMaterialName( STRINGIZE_STRING_LOCAL( "TextureArray_Blend" ) );
        tiledMap->setResourceTiledMap( mapResource );

        m_virtualArea->addContentNode( tiledMap, false );
        m_tiledMap = tiledMap;

        this->updateContentBounds_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceImageDefaultPtr TiledMapExampleSceneEventReceiver::createImageResource_( const ConstString & _name, const FilePath & _path )
    {
        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getFileGroup( STRINGIZE_STRING_LOCAL( "TiledExamples" ) );

        if( fileGroup == nullptr || fileGroup->existFile( _path, true ) == false )
        {
            return nullptr;
        }

        const ConstString & codecType = CODEC_SERVICE()
            ->findCodecType( _path );

        if( codecType.empty() == true )
        {
            return nullptr;
        }

        ContentInterfacePtr content = Helper::makeFileContent( fileGroup, _path, MENGINE_DOCUMENT_FACTORABLE );
        content->setCodecType( codecType );

        RenderImageLoaderInterfacePtr loader = RENDERTEXTURE_SERVICE()
            ->createDecoderRenderImageLoader( content, DF_IMAGE_NONE, MENGINE_DOCUMENT_FACTORABLE );

        if( loader == nullptr )
        {
            return nullptr;
        }

        RenderImageDesc imageDesc;
        loader->getImageDesc( &imageDesc );

        if( imageDesc.width == 0 || imageDesc.height == 0 )
        {
            return nullptr;
        }

        ResourceCook cook;
        cook.groupName = STRINGIZE_STRING_LOCAL( "TiledExamples" );
        cook.name = _name;
        cook.type = ResourceImageDefault::getFactorableType();

        ResourceImageDefaultPtr resource = RESOURCE_SERVICE()
            ->createResource( cook, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( resource, "invalid create image resource '%s'", _name.c_str() );

        const mt::vec2f imageSize( (float)imageDesc.width, (float)imageDesc.height );
        const mt::uv4f identityUV;

        resource->setContent( content );
        resource->setMaxSize( imageSize );
        resource->setSize( imageSize );
        resource->setOffset( mt::vec2f( 0.f, 0.f ) );
        resource->setUV( 0, identityUV );
        resource->setUV( 1, identityUV );

        if( resource->initialize() == false )
        {
            RESOURCE_SERVICE()
                ->removeResource( resource );

            return nullptr;
        }

        m_resources.emplace_back( resource );

        return resource;
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceTiledMapPtr TiledMapExampleSceneEventReceiver::createMapResource_( const FilePath & _path )
    {
        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getFileGroup( STRINGIZE_STRING_LOCAL( "TiledExamples" ) );

        if( fileGroup == nullptr || fileGroup->existFile( _path, true ) == false )
        {
            return nullptr;
        }

        ResourceCook cook;
        cook.groupName = STRINGIZE_STRING_LOCAL( "TiledExamples" );
        cook.type = ResourceTiledMap::getFactorableType();

        ResourceTiledMapPtr resource = RESOURCE_SERVICE()
            ->createResource( cook, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( resource, "invalid create Tiled map resource '%s'", _path.c_str() );

        ContentInterfacePtr content = Helper::makeFileContent( fileGroup, _path, MENGINE_DOCUMENT_FACTORABLE );
        resource->setContent( content );

        if( resource->initialize() == false )
        {
            RESOURCE_SERVICE()
                ->removeResource( resource );

            return nullptr;
        }

        if( resource->compile() == false )
        {
            RESOURCE_SERVICE()
                ->removeResource( resource );

            return nullptr;
        }

        m_resources.emplace_back( resource );

        return resource;
    }
    //////////////////////////////////////////////////////////////////////////
    void TiledMapExampleSceneEventReceiver::updateContentBounds_()
    {
        if( m_virtualArea == nullptr || m_tiledMap == nullptr )
        {
            return;
        }

        mt::vec2f minimum( 0.f, 0.f );
        mt::vec2f maximum;

        if( m_tiledMap->isCompile() == true )
        {
            const mt::box2f & boundingBox = m_tiledMap->getMapBoundingBox();
            minimum = boundingBox.minimum;
            maximum = boundingBox.maximum;
        }
        else
        {
            const float columns = (float)m_tiledMap->getColumnCount();
            const float rows = (float)m_tiledMap->getRowCount();
            const float tileWidth = (float)m_tiledMap->getTileWidth();
            const float tileHeight = (float)m_tiledMap->getTileHeight();

            if( m_tiledMap->getOrientation() == ETMO_ISOMETRIC )
            {
                maximum.x = (columns + rows) * tileWidth * 0.5f;
                maximum.y = (columns + rows) * tileHeight * 0.5f;
            }
            else if( m_tiledMap->getOrientation() == ETMO_HEXAGONAL )
            {
                maximum.x = columns * tileWidth + tileWidth * 0.5f;
                maximum.y = rows * tileHeight + tileHeight * 0.5f;
            }
            else
            {
                maximum.x = columns * tileWidth;
                maximum.y = rows * tileHeight;
            }
        }

        const float padding = 48.f;
        m_virtualArea->setContentSize( minimum.x - padding, minimum.y - padding, maximum.x + padding, maximum.y + padding );
        m_virtualArea->setScale( 1.f );
        m_virtualArea->setPercentage( mt::vec2f( 0.f, 0.f ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void TiledMapExampleSceneEventReceiver::clearMap_()
    {
        if( m_tiledMap != nullptr )
        {
            m_tiledMap->dispose();
            m_tiledMap = nullptr;
        }

        for( VectorResources::reverse_iterator it = m_resources.rbegin(); it != m_resources.rend(); ++it )
        {
            if( (*it)->isCompile() == true )
            {
                (*it)->release();
            }

            RESOURCE_SERVICE()
                ->removeResource( *it );
        }

        m_resources.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void TiledMapExampleSceneEventReceiver::renderControls_( const ImGUIRenderProviderInterfacePtr & _provider )
    {
        MENGINE_UNUSED( _provider );

        ImGui::SetNextWindowPos( ImVec2( 16.f, 12.f ), ImGuiCond_Always );
        ImGui::SetNextWindowSize( ImVec2( 980.f, 76.f ), ImGuiCond_Always );
        ImGui::SetNextWindowBgAlpha( 0.92f );

        const ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;

        if( ImGui::Begin( "Official Tiled maps / Texture2DArray", nullptr, flags ) == false )
        {
            ImGui::End();

            return;
        }

        if( ImGui::Button( "Previous" ) == true )
        {
            this->loadMap_( (m_mapIndex + (uint32_t)m_examples.size() - 1) % (uint32_t)m_examples.size() );
        }

        ImGui::SameLine();

        if( ImGui::Button( "Next" ) == true )
        {
            this->loadMap_( (m_mapIndex + 1) % (uint32_t)m_examples.size() );
        }

        ImGui::SameLine();
        ImGui::Text( "%u/%u  %s  [excluded: %u]", m_mapIndex + 1, (uint32_t)m_examples.size(), m_examples[m_mapIndex].title.c_str(), m_excludedMapCount );

        if( m_tiledMap != nullptr )
        {
            ImGui::Text( "%s  %ux%u  tiles: %u  batches: %u  arrays: %u  array layers: %u  texture memory: %llu bytes",
                Detail::s_orientationName( m_tiledMap->getOrientation() ),
                m_tiledMap->getColumnCount(),
                m_tiledMap->getRowCount(),
                m_tiledMap->getTileCount(),
                m_tiledMap->getBatchCount(),
                m_tiledMap->getTextureArrayCount(),
                m_tiledMap->getTextureArrayLayerCount(),
                (unsigned long long)m_tiledMap->getResidentTextureMemoryBytes() );
        }
        else if( m_error.empty() == false )
        {
            ImGui::TextUnformatted( m_error.c_str() );
        }

        ImGui::End();
    }
    //////////////////////////////////////////////////////////////////////////
}
