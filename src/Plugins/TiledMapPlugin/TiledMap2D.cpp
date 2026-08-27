#include "TiledMap2D.h"

#include "Interface/RenderMaterialServiceInterface.h"
#include "Interface/RenderPipelineInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/RenderCameraInterface.h"
#include "Interface/RenderTextureServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/MemoryCopy.h"
#include "Kernel/PixelFormatHelper.h"
#include "Kernel/ResourceCast.h"
#include "Kernel/VectorRenderIndex.h"
#include "Kernel/VectorRenderVertex2D.h"

#include "Config/StdAlgorithm.h"

#include <cmath>
#include <cstring>
#include <limits>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TiledMap2D::TiledMap2D()
        : m_vertexCount( 0 )
        , m_indexCount( 0 )
        , m_textureArrayLayerCount( 0 )
        , m_materialName( STRINGIZE_STRING_LOCAL( "TextureArray_Blend" ) )
    {
        mt::box2_reset( &m_mapBoundingBox, 0.f, 0.f );
        mt::ident_m4( &m_renderWorldMatrix );
    }
    //////////////////////////////////////////////////////////////////////////
    TiledMap2D::~TiledMap2D()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void TiledMap2D::setResourceTiledMap( const ResourceTiledMapPtr & _resource )
    {
        if( m_resourceTiledMap == _resource )
        {
            return;
        }

        this->recompile( [this, &_resource]()
        {
            m_resourceTiledMap = _resource;

            return m_resourceTiledMap != nullptr;
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceTiledMapPtr & TiledMap2D::getResourceTiledMap() const
    {
        return m_resourceTiledMap;
    }
    //////////////////////////////////////////////////////////////////////////
    void TiledMap2D::setMaterialName( const ConstString & _materialName )
    {
        if( this->isCompile() == true )
        {
            LOGGER_ERROR( "tiledmap2d '%s' material is immutable after compile"
                , this->getName().c_str()
            );

            return;
        }

        m_materialName = _materialName;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & TiledMap2D::getMaterialName() const
    {
        return m_materialName;
    }
    //////////////////////////////////////////////////////////////////////////
    ETiledMapOrientation TiledMap2D::getOrientation() const
    {
        return m_resourceTiledMap != nullptr ? m_resourceTiledMap->getOrientation() : ETMO_ORTHOGONAL;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TiledMap2D::getColumnCount() const
    {
        return m_resourceTiledMap != nullptr ? m_resourceTiledMap->getColumnCount() : 0;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TiledMap2D::getRowCount() const
    {
        return m_resourceTiledMap != nullptr ? m_resourceTiledMap->getRowCount() : 0;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TiledMap2D::getTileWidth() const
    {
        return m_resourceTiledMap != nullptr ? m_resourceTiledMap->getTileWidth() : 0;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TiledMap2D::getTileHeight() const
    {
        return m_resourceTiledMap != nullptr ? m_resourceTiledMap->getTileHeight() : 0;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TiledMap2D::getTileCount() const
    {
        return m_resourceTiledMap != nullptr ? m_resourceTiledMap->getTileCount() : 0;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TiledMap2D::getTileLayerCount() const
    {
        return m_resourceTiledMap != nullptr ? (uint32_t)m_resourceTiledMap->getTileLayers().size() : 0;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TiledMap2D::getObjectLayerCount() const
    {
        return m_resourceTiledMap != nullptr ? (uint32_t)m_resourceTiledMap->getObjectLayers().size() : 0;
    }
    //////////////////////////////////////////////////////////////////////////
    const String & TiledMap2D::getTileLayerName( uint32_t _index ) const
    {
        if( m_resourceTiledMap == nullptr || _index >= m_resourceTiledMap->getTileLayers().size() )
        {
            static const String empty;

            return empty;
        }

        return m_resourceTiledMap->getTileLayers()[_index].name;
    }
    //////////////////////////////////////////////////////////////////////////
    const String & TiledMap2D::getObjectLayerName( uint32_t _index ) const
    {
        if( m_resourceTiledMap == nullptr || _index >= m_resourceTiledMap->getObjectLayers().size() )
        {
            static const String empty;

            return empty;
        }

        return m_resourceTiledMap->getObjectLayers()[_index].name;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TiledMap2D::setTileLayerVisible( uint32_t _index, bool _visible )
    {
        if( _index >= m_tileLayerVisibilities.size() )
        {
            return false;
        }

        m_tileLayerVisibilities[_index] = _visible;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TiledMap2D::isTileLayerVisible( uint32_t _index ) const
    {
        return _index < m_tileLayerVisibilities.size() && m_tileLayerVisibilities[_index];
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TiledMap2D::getObjectCount( uint32_t _layerIndex ) const
    {
        if( m_resourceTiledMap == nullptr || _layerIndex >= m_resourceTiledMap->getObjectLayers().size() )
        {
            return 0;
        }

        return (uint32_t)m_resourceTiledMap->getObjectLayers()[_layerIndex].objects.size();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TiledMap2D::getObjectId( uint32_t _layerIndex, uint32_t _objectIndex ) const
    {
        const TiledMapObject * object = m_resourceTiledMap != nullptr ? m_resourceTiledMap->getObject( _layerIndex, _objectIndex ) : nullptr;

        return object != nullptr ? object->id : 0;
    }
    //////////////////////////////////////////////////////////////////////////
    const String & TiledMap2D::getObjectName( uint32_t _layerIndex, uint32_t _objectIndex ) const
    {
        const TiledMapObject * object = m_resourceTiledMap != nullptr ? m_resourceTiledMap->getObject( _layerIndex, _objectIndex ) : nullptr;

        if( object == nullptr )
        {
            static const String empty;

            return empty;
        }

        return object->name;
    }
    //////////////////////////////////////////////////////////////////////////
    const String & TiledMap2D::getObjectType( uint32_t _layerIndex, uint32_t _objectIndex ) const
    {
        const TiledMapObject * object = m_resourceTiledMap != nullptr ? m_resourceTiledMap->getObject( _layerIndex, _objectIndex ) : nullptr;

        if( object == nullptr )
        {
            static const String empty;

            return empty;
        }

        return object->type;
    }
    //////////////////////////////////////////////////////////////////////////
    const String & TiledMap2D::getObjectClass( uint32_t _layerIndex, uint32_t _objectIndex ) const
    {
        const TiledMapObject * object = m_resourceTiledMap != nullptr ? m_resourceTiledMap->getObject( _layerIndex, _objectIndex ) : nullptr;

        if( object == nullptr )
        {
            static const String empty;

            return empty;
        }

        return object->className;
    }
    //////////////////////////////////////////////////////////////////////////
    ETiledMapObjectShape TiledMap2D::getObjectShape( uint32_t _layerIndex, uint32_t _objectIndex ) const
    {
        const TiledMapObject * object = m_resourceTiledMap != nullptr ? m_resourceTiledMap->getObject( _layerIndex, _objectIndex ) : nullptr;

        return object != nullptr ? object->shape : ETMOS_RECTANGLE;
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f TiledMap2D::getObjectPosition( uint32_t _layerIndex, uint32_t _objectIndex ) const
    {
        const TiledMapObject * object = m_resourceTiledMap != nullptr ? m_resourceTiledMap->getObject( _layerIndex, _objectIndex ) : nullptr;

        return object != nullptr ? object->position : mt::vec2f( 0.f, 0.f );
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f TiledMap2D::getObjectSize( uint32_t _layerIndex, uint32_t _objectIndex ) const
    {
        const TiledMapObject * object = m_resourceTiledMap != nullptr ? m_resourceTiledMap->getObject( _layerIndex, _objectIndex ) : nullptr;

        return object != nullptr ? object->size : mt::vec2f( 0.f, 0.f );
    }
    //////////////////////////////////////////////////////////////////////////
    float TiledMap2D::getObjectRotation( uint32_t _layerIndex, uint32_t _objectIndex ) const
    {
        const TiledMapObject * object = m_resourceTiledMap != nullptr ? m_resourceTiledMap->getObject( _layerIndex, _objectIndex ) : nullptr;

        return object != nullptr ? object->rotation : 0.f;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TiledMap2D::getObjectPointCount( uint32_t _layerIndex, uint32_t _objectIndex ) const
    {
        const TiledMapObject * object = m_resourceTiledMap != nullptr ? m_resourceTiledMap->getObject( _layerIndex, _objectIndex ) : nullptr;

        return object != nullptr ? (uint32_t)object->points.size() : 0;
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f TiledMap2D::getObjectPoint( uint32_t _layerIndex, uint32_t _objectIndex, uint32_t _pointIndex ) const
    {
        const TiledMapObject * object = m_resourceTiledMap != nullptr ? m_resourceTiledMap->getObject( _layerIndex, _objectIndex ) : nullptr;

        if( object == nullptr || _pointIndex >= object->points.size() )
        {
            return mt::vec2f( 0.f, 0.f );
        }

        return object->points[_pointIndex];
    }
    //////////////////////////////////////////////////////////////////////////
    TiledMapProperties TiledMap2D::getMapProperties() const
    {
        return m_resourceTiledMap != nullptr ? m_resourceTiledMap->getMapProperties() : TiledMapProperties();
    }
    //////////////////////////////////////////////////////////////////////////
    TiledMapProperties TiledMap2D::getTileLayerProperties( uint32_t _layerIndex ) const
    {
        if( m_resourceTiledMap == nullptr || _layerIndex >= m_resourceTiledMap->getTileLayers().size() )
        {
            return TiledMapProperties();
        }

        return m_resourceTiledMap->getTileLayers()[_layerIndex].properties;
    }
    //////////////////////////////////////////////////////////////////////////
    TiledMapProperties TiledMap2D::getObjectLayerProperties( uint32_t _layerIndex ) const
    {
        if( m_resourceTiledMap == nullptr || _layerIndex >= m_resourceTiledMap->getObjectLayers().size() )
        {
            return TiledMapProperties();
        }

        return m_resourceTiledMap->getObjectLayers()[_layerIndex].properties;
    }
    //////////////////////////////////////////////////////////////////////////
    TiledMapProperties TiledMap2D::getObjectProperties( uint32_t _layerIndex, uint32_t _objectIndex ) const
    {
        const TiledMapObject * object = m_resourceTiledMap != nullptr ? m_resourceTiledMap->getObject( _layerIndex, _objectIndex ) : nullptr;

        return object != nullptr ? object->properties : TiledMapProperties();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TiledMap2D::getBatchCount() const
    {
        return (uint32_t)m_batches.size();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TiledMap2D::getVertexCount() const
    {
        return m_vertexCount;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TiledMap2D::getIndexCount() const
    {
        return m_indexCount;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TiledMap2D::getTextureArrayCount() const
    {
        return (uint32_t)m_textureArrays.size();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TiledMap2D::getTextureArrayLayerCount() const
    {
        return m_textureArrayLayerCount;
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t TiledMap2D::getResidentTextureMemoryBytes() const
    {
        uint64_t bytes = 0;

        for( const RenderTextureInterfacePtr & texture : m_textureArrays )
        {
            const RenderImageInterfacePtr & image = texture->getImage();
            uint32_t channels = Helper::getPixelFormatChannels( image->getHWPixelFormat() );
            bytes += (uint64_t)image->getHWWidth() * (uint64_t)image->getHWHeight() * (uint64_t)image->getHWLayerCount() * (uint64_t)channels;
        }

        return bytes;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::box2f & TiledMap2D::getMapBoundingBox() const
    {
        return m_mapBoundingBox;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::mat4f & TiledMap2D::getTransformationWorldMatrix() const
    {
        return m_renderWorldMatrix;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TiledMap2D::_compile()
    {
        this->releaseBatches_();
        m_tileLayerVisibilities.clear();

        if( m_resourceTiledMap == nullptr )
        {
            LOGGER_ERROR( "tiledmap2d '%s' requires a Tiled map resource"
                , this->getName().c_str()
            );

            return false;
        }

        if( m_resourceTiledMap->compile() == false )
        {
            LOGGER_ERROR( "tiledmap2d '%s' can't compile resource '%s'"
                , this->getName().c_str()
                , m_resourceTiledMap->getName().c_str()
            );

            return false;
        }

        const VectorTiledMapTileLayers & tileLayers = m_resourceTiledMap->getTileLayers();
        m_tileLayerVisibilities.reserve( tileLayers.size() );

        for( const TiledMapTileLayer & layer : tileLayers )
        {
            m_tileLayerVisibilities.emplace_back( layer.visible );
        }

        if( this->compileBatches_() == false )
        {
            m_tileLayerVisibilities.clear();
            m_resourceTiledMap->release();

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TiledMap2D::compileBatches_()
    {
        const VectorTiledMapTilesets & tilesets = m_resourceTiledMap->getTilesets();
        const VectorTiledMapTileLayers & tileLayers = m_resourceTiledMap->getTileLayers();
        const ETiledMapOrientation orientation = m_resourceTiledMap->getOrientation();
        const uint32_t mapHeight = m_resourceTiledMap->getRowCount();
        const uint32_t tileWidth = m_resourceTiledMap->getTileWidth();
        const uint32_t tileHeight = m_resourceTiledMap->getTileHeight();
        const uint32_t hexSideLength = m_resourceTiledMap->getHexSideLength();
        const uint32_t staggerAxis = m_resourceTiledMap->getStaggerAxis();
        const uint32_t staggerIndex = m_resourceTiledMap->getStaggerIndex();

        struct TextureArraySource
        {
            String resourceName;
            Optional<uint32_t> transparentColor;
            ResourceImagePtr resource;
            RenderImageLoaderInterfacePtr loader;
            RenderImageDesc desc;
            uint32_t textureArrayIndex;
            uint32_t textureArrayLayer;
        };

        typedef Vector<TextureArraySource> VectorTextureArraySources;
        typedef Vector<uint32_t> VectorTextureArraySourceIndices;

        struct TextureArrayResource
        {
            String resourceName;
            Optional<uint32_t> transparentColor;
        };

        typedef Vector<TextureArrayResource> VectorTextureArrayResources;

        struct TextureArrayBuild
        {
            uint32_t width;
            uint32_t height;
            EPixelFormat format;
            VectorTextureArraySourceIndices sourceIndices;
            RenderTextureInterfacePtr texture;
        };

        typedef Vector<TextureArrayBuild> VectorTextureArrayBuilds;

        VectorTextureArraySources textureArraySources;
        VectorTextureArrayBuilds textureArrayBuilds;

        uint32_t maxTextureArrayLayers = RENDER_SYSTEM()->getMaxTexture2DArrayLayers();

        if( maxTextureArrayLayers < 2 )
        {
            LOGGER_ERROR( "tiledmap2d '%s' renderer doesn't support texture arrays"
                , this->getName().c_str()
            );

            return false;
        }

        VectorTextureArrayResources textureArrayResources;

        for( const TiledMapTileset & tileset : tilesets )
        {
            if( tileset.resourceName.empty() == false )
            {
                TextureArrayResource resource;
                resource.resourceName = tileset.resourceName;
                resource.transparentColor = tileset.transparentColor;
                textureArrayResources.emplace_back( std::move( resource ) );
            }

            for( const TiledMapTileImage & image : tileset.tileImages )
            {
                TextureArrayResource resource;
                resource.resourceName = image.resourceName;
                textureArrayResources.emplace_back( std::move( resource ) );
            }
        }

        const ConstString & groupName = m_resourceTiledMap->getGroupName();

        for( const TextureArrayResource & textureArrayResource : textureArrayResources )
        {
            bool duplicate = false;

            for( const TextureArraySource & source : textureArraySources )
            {
                if( source.resourceName == textureArrayResource.resourceName && source.transparentColor == textureArrayResource.transparentColor )
                {
                    duplicate = true;

                    break;
                }
            }

            if( duplicate == true )
            {
                continue;
            }

            ResourcePtr baseResource = RESOURCE_SERVICE()
                ->getResourceReference( groupName, Helper::stringizeString( textureArrayResource.resourceName ) );

            ResourceImagePtr resource = Helper::dynamicResourceCast<ResourceImagePtr>( baseResource );

            if( resource == nullptr || resource->getContent() == nullptr )
            {
                LOGGER_ERROR( "tiledmap2d '%s' texture array source resource '%s' has no image content"
                    , this->getName().c_str()
                    , textureArrayResource.resourceName.c_str()
                );

                return false;
            }

            RenderImageLoaderInterfacePtr loader = RENDERTEXTURE_SERVICE()
                ->createDecoderRenderImageLoader( resource->getContent(), DF_IMAGE_NONE, MENGINE_DOCUMENT_FACTORABLE );

            if( loader == nullptr )
            {
                LOGGER_ERROR( "tiledmap2d '%s' can't create decoder for texture array source '%s'"
                    , this->getName().c_str()
                    , textureArrayResource.resourceName.c_str()
                );

                return false;
            }

            RenderImageDesc desc;
            loader->getImageDesc( &desc );

            if( desc.width == 0 || desc.height == 0 || desc.format == PF_UNKNOWN )
            {
                LOGGER_ERROR( "tiledmap2d '%s' texture array source '%s' has invalid image description"
                    , this->getName().c_str()
                    , textureArrayResource.resourceName.c_str()
                );

                return false;
            }

            uint32_t textureArrayIndex = (uint32_t)textureArrayBuilds.size();

            for( uint32_t index = 0; index != (uint32_t)textureArrayBuilds.size(); ++index )
            {
                const TextureArrayBuild & textureArray = textureArrayBuilds[index];

                if( textureArray.width == desc.width &&
                    textureArray.height == desc.height &&
                    textureArray.format == desc.format &&
                    textureArray.sourceIndices.size() < maxTextureArrayLayers )
                {
                    textureArrayIndex = index;

                    break;
                }
            }

            if( textureArrayIndex == textureArrayBuilds.size() )
            {
                TextureArrayBuild textureArray;
                textureArray.width = desc.width;
                textureArray.height = desc.height;
                textureArray.format = desc.format;
                textureArrayBuilds.emplace_back( std::move( textureArray ) );
            }

            TextureArrayBuild & textureArray = textureArrayBuilds[textureArrayIndex];

            TextureArraySource source;
            source.resourceName = textureArrayResource.resourceName;
            source.transparentColor = textureArrayResource.transparentColor;
            source.resource = resource;
            source.loader = loader;
            source.desc = desc;
            source.textureArrayIndex = textureArrayIndex;
            source.textureArrayLayer = (uint32_t)textureArray.sourceIndices.size();

            textureArray.sourceIndices.emplace_back( (uint32_t)textureArraySources.size() );
            textureArraySources.emplace_back( std::move( source ) );
        }

        struct BuildBatch
        {
            uint32_t layerIndex;
            int32_t chunkX;
            int32_t chunkY;
            uint32_t textureArrayIndex;
            VectorRenderVertex2D vertices;
            VectorRenderIndex indices;
        };

        typedef Vector<BuildBatch> VectorBuildBatches;

        VectorBuildBatches buildBatches;
        const int32_t chunkSize = 32;
        const uint32_t maximumQuads = (uint32_t)std::numeric_limits<RenderIndex>::max() / 4U;
        const uint32_t flipHorizontal = 0x80000000U;
        const uint32_t flipVertical = 0x40000000U;
        const uint32_t flipDiagonal = 0x20000000U;
        const uint32_t gidMask = ~(flipHorizontal | flipVertical | flipDiagonal | 0x10000000U);

        for( uint32_t layerIndex = 0; layerIndex != (uint32_t)tileLayers.size(); ++layerIndex )
        {
            const TiledMapTileLayer & layer = tileLayers[layerIndex];

            for( const TiledMapTile & tile : layer.tiles )
            {
                uint32_t gid = tile.gid & gidMask;

                if( gid == 0 )
                {
                    continue;
                }

                const TiledMapTileset * tileset = nullptr;

                for( const TiledMapTileset & candidate : tilesets )
                {
                    if( candidate.firstGid <= gid && (tileset == nullptr || candidate.firstGid > tileset->firstGid) )
                    {
                        tileset = &candidate;
                    }
                }

                if( tileset == nullptr )
                {
                    LOGGER_ERROR( "tiledmap2d '%s' tile gid %u has no tileset"
                        , this->getName().c_str()
                        , gid
                    );

                    this->releaseBatches_();

                    return false;
                }

                uint32_t localId = gid - tileset->firstGid;
                String resourceName = tileset->resourceName;
                Optional<uint32_t> transparentColor = tileset->transparentColor;
                uint32_t imageWidth = tileset->imageWidth;
                uint32_t imageHeight = tileset->imageHeight;
                uint32_t columns = tileset->columns;
                uint32_t tileCount = tileset->tileCount;
                uint32_t sourceX = tileset->margin;
                uint32_t sourceY = tileset->margin;
                uint32_t sourceWidth = tileset->tileWidth;
                uint32_t sourceHeight = tileset->tileHeight;

                for( const TiledMapTileImage & image : tileset->tileImages )
                {
                    if( image.localId == localId )
                    {
                        resourceName = image.resourceName;
                        transparentColor.reset();
                        imageWidth = image.imageWidth;
                        imageHeight = image.imageHeight;
                        sourceX = image.sourceX;
                        sourceY = image.sourceY;
                        sourceWidth = image.sourceWidth;
                        sourceHeight = image.sourceHeight;
                        break;
                    }
                }

                const TextureArraySource * textureArraySource = nullptr;

                for( const TextureArraySource & source : textureArraySources )
                {
                    if( source.resourceName == resourceName && source.transparentColor == transparentColor )
                    {
                        textureArraySource = &source;

                        break;
                    }
                }

                if( textureArraySource == nullptr )
                {
                    LOGGER_ERROR( "tiledmap2d '%s' can't find texture array source '%s'"
                        , this->getName().c_str()
                        , resourceName.c_str()
                    );

                    this->releaseBatches_();

                    return false;
                }

                if( imageWidth == 0 )
                {
                    imageWidth = textureArraySource->desc.width;
                }

                if( imageHeight == 0 )
                {
                    imageHeight = textureArraySource->desc.height;
                }

                if( sourceWidth == 0 )
                {
                    sourceWidth = imageWidth;
                }

                if( sourceHeight == 0 )
                {
                    sourceHeight = imageHeight;
                }

                if( columns == 0 && tileset->tileWidth != 0 )
                {
                    int64_t numerator = (int64_t)imageWidth - (int64_t)tileset->margin * 2 + tileset->spacing;
                    int64_t denominator = (int64_t)tileset->tileWidth + tileset->spacing;
                    columns = numerator > 0 && denominator > 0 ? (uint32_t)(numerator / denominator) : 0;
                }

                if( tileCount == 0 && columns != 0 && tileset->tileHeight != 0 )
                {
                    int64_t numerator = (int64_t)imageHeight - (int64_t)tileset->margin * 2 + tileset->spacing;
                    int64_t denominator = (int64_t)tileset->tileHeight + tileset->spacing;
                    int64_t rows = numerator > 0 && denominator > 0 ? numerator / denominator : 0;
                    uint64_t inferredCount = (uint64_t)columns * (uint64_t)rows;

                    if( inferredCount <= std::numeric_limits<uint32_t>::max() )
                    {
                        tileCount = (uint32_t)inferredCount;
                    }
                }

                if( localId >= tileCount || resourceName.empty() == true || imageWidth == 0 || imageHeight == 0 || sourceWidth == 0 || sourceHeight == 0 )
                {
                    LOGGER_ERROR( "tiledmap2d '%s' tile gid %u has invalid image metadata"
                        , this->getName().c_str()
                        , gid
                    );

                    this->releaseBatches_();

                    return false;
                }

                if( columns != 0 && tileset->tileImages.empty() == true )
                {
                    sourceX += localId % columns * (tileset->tileWidth + tileset->spacing);
                    sourceY += localId / columns * (tileset->tileHeight + tileset->spacing);
                }

                BuildBatch * batch = nullptr;
                int32_t chunkX = (int32_t)std::floor( (float)tile.x / (float)chunkSize );
                int32_t chunkY = (int32_t)std::floor( (float)tile.y / (float)chunkSize );

                for( BuildBatch & candidate : buildBatches )
                {
                    if( candidate.layerIndex == layerIndex &&
                        candidate.chunkX == chunkX &&
                        candidate.chunkY == chunkY &&
                        candidate.textureArrayIndex == textureArraySource->textureArrayIndex &&
                        candidate.vertices.size() / 4U < maximumQuads )
                    {
                        batch = &candidate;

                        break;
                    }
                }

                if( batch == nullptr )
                {
                    buildBatches.emplace_back();
                    batch = &buildBatches.back();
                    batch->layerIndex = layerIndex;
                    batch->chunkX = chunkX;
                    batch->chunkY = chunkY;
                    batch->textureArrayIndex = textureArraySource->textureArrayIndex;
                }

                float cellX;
                float cellY;

                if( orientation == ETMO_ISOMETRIC )
                {
                    cellX = ((float)tile.x - (float)tile.y) * (float)tileWidth * 0.5f + (float)mapHeight * (float)tileWidth * 0.5f;
                    cellY = ((float)tile.x + (float)tile.y) * (float)tileHeight * 0.5f;
                    cellX -= ((float)sourceWidth - (float)tileWidth) * 0.5f;
                }
                else if( orientation == ETMO_HEXAGONAL && staggerAxis == 0 )
                {
                    float step = ((float)tileWidth + (float)hexSideLength) * 0.5f;
                    bool stagger = ((tile.x & 1) != 0) == (staggerIndex != 0);
                    cellX = (float)tile.x * step;
                    cellY = (float)tile.y * (float)tileHeight + (stagger == true ? (float)tileHeight * 0.5f : 0.f);
                }
                else if( orientation == ETMO_HEXAGONAL )
                {
                    float step = ((float)tileHeight + (float)hexSideLength) * 0.5f;
                    bool stagger = ((tile.y & 1) != 0) == (staggerIndex != 0);
                    cellX = (float)tile.x * (float)tileWidth + (stagger == true ? (float)tileWidth * 0.5f : 0.f);
                    cellY = (float)tile.y * step;
                }
                else
                {
                    cellX = (float)tile.x * (float)tileWidth;
                    cellY = (float)tile.y * (float)tileHeight;
                }

                cellX += layer.offset.x + tileset->offset.x;
                cellY += layer.offset.y + tileset->offset.y + (float)tileHeight - (float)sourceHeight;

                uint32_t vertexOffset = (uint32_t)batch->vertices.size();
                uint32_t indexOffset = (uint32_t)batch->indices.size();
                batch->vertices.resize( vertexOffset + 4 );
                batch->indices.resize( indexOffset + 6 );
                float opacity = mt::clamp( 0.f, layer.opacity, 1.f );
                ColorValue_ARGB color = ((uint32_t)(opacity * 255.f) << 24) | 0x00FFFFFFU;
                const float positionX[4] = {0.f, (float)sourceWidth, (float)sourceWidth, 0.f};
                const float positionY[4] = {0.f, 0.f, (float)sourceHeight, (float)sourceHeight};
                const float cornerX[4] = {0.f, 1.f, 1.f, 0.f};
                const float cornerY[4] = {0.f, 0.f, 1.f, 1.f};
                const mt::vec2f sourceUVBegin(
                    ((float)sourceX + 0.5f) / (float)imageWidth,
                    ((float)sourceY + 0.5f) / (float)imageHeight
                );
                const mt::vec2f sourceUVEnd(
                    ((float)sourceX + (float)sourceWidth - 0.5f) / (float)imageWidth,
                    ((float)sourceY + (float)sourceHeight - 0.5f) / (float)imageHeight
                );

                for( uint32_t vertexIndex = 0; vertexIndex != 4; ++vertexIndex )
                {
                    RenderVertex2D & vertex = batch->vertices[vertexOffset + vertexIndex];
                    float localX = positionX[vertexIndex];
                    float localY = positionY[vertexIndex];

                    if( orientation == ETMO_HEXAGONAL )
                    {
                        uint32_t rotationSteps = ((tile.gid & flipDiagonal) != 0 ? 1U : 0U) + ((tile.gid & 0x10000000U) != 0 ? 2U : 0U);

                        if( rotationSteps != 0 )
                        {
                            const float pi = 3.14159265358979323846f;
                            float angle = (float)rotationSteps * pi / 3.f;
                            float cosine = std::cos( angle );
                            float sine = std::sin( angle );
                            float dx = localX - (float)sourceWidth * 0.5f;
                            float dy = localY - (float)sourceHeight * 0.5f;
                            localX = dx * cosine - dy * sine + (float)sourceWidth * 0.5f;
                            localY = dx * sine + dy * cosine + (float)sourceHeight * 0.5f;
                        }
                    }

                    vertex.position = mt::vec3f( cellX + localX, cellY + localY, 0.f );
                    vertex.color = color;
                    float sourceCornerX = cornerX[vertexIndex];
                    float sourceCornerY = cornerY[vertexIndex];

                    if( (tile.gid & flipVertical) != 0 )
                    {
                        sourceCornerY = 1.f - sourceCornerY;
                    }

                    if( (tile.gid & flipHorizontal) != 0 )
                    {
                        sourceCornerX = 1.f - sourceCornerX;
                    }

                    if( orientation != ETMO_HEXAGONAL && (tile.gid & flipDiagonal) != 0 )
                    {
                        float previousX = sourceCornerX;
                        sourceCornerX = 1.f - sourceCornerY;
                        sourceCornerY = 1.f - previousX;
                    }

                    const mt::vec2f sourceUV(
                        sourceUVBegin.x + (sourceUVEnd.x - sourceUVBegin.x) * sourceCornerX,
                        sourceUVBegin.y + (sourceUVEnd.y - sourceUVBegin.y) * sourceCornerY
                    );

                    const mt::uv4f & resourceUV = textureArraySource->resource->getUVTexture( 0 );
                    mt::vec2f top;
                    top.x = resourceUV[0].x + (resourceUV[1].x - resourceUV[0].x) * sourceUV.x;
                    top.y = resourceUV[0].y + (resourceUV[1].y - resourceUV[0].y) * sourceUV.x;
                    mt::vec2f bottom;
                    bottom.x = resourceUV[3].x + (resourceUV[2].x - resourceUV[3].x) * sourceUV.x;
                    bottom.y = resourceUV[3].y + (resourceUV[2].y - resourceUV[3].y) * sourceUV.x;
                    vertex.uv[0].x = top.x + (bottom.x - top.x) * sourceUV.y;
                    vertex.uv[0].y = top.y + (bottom.y - top.y) * sourceUV.y;
                    vertex.uv[1] = mt::vec2f( (float)textureArraySource->textureArrayLayer, 0.f );
                }

                batch->indices[indexOffset + 0] = (RenderIndex)(vertexOffset + 0);
                batch->indices[indexOffset + 1] = (RenderIndex)(vertexOffset + 1);
                batch->indices[indexOffset + 2] = (RenderIndex)(vertexOffset + 2);
                batch->indices[indexOffset + 3] = (RenderIndex)(vertexOffset + 0);
                batch->indices[indexOffset + 4] = (RenderIndex)(vertexOffset + 2);
                batch->indices[indexOffset + 5] = (RenderIndex)(vertexOffset + 3);
            }
        }

        m_vertexCount = 0;
        m_indexCount = 0;
        m_textureArrayLayerCount = 0;
        mt::box2_insideout( &m_mapBoundingBox );
        m_batches.reserve( buildBatches.size() );
        m_textureArrays.reserve( textureArrayBuilds.size() );

        for( TextureArrayBuild & textureArray : textureArrayBuilds )
        {
            uint32_t sourceLayerCount = (uint32_t)textureArray.sourceIndices.size();
            uint32_t hardwareLayerCount = StdAlgorithm::max( sourceLayerCount, 2U );

            RenderImageInterfacePtr image = RENDER_SYSTEM()
                ->createImage( 1, textureArray.width, textureArray.height, hardwareLayerCount, textureArray.format, MENGINE_DOCUMENT_FACTORABLE );

            if( image == nullptr )
            {
                LOGGER_ERROR( "tiledmap2d '%s' can't create texture array %ux%ux%u format %u"
                    , this->getName().c_str()
                    , textureArray.width
                    , textureArray.height
                    , hardwareLayerCount
                    , (uint32_t)textureArray.format
                );

                this->releaseBatches_();

                return false;
            }

            RenderTextureInterfacePtr texture = RENDERTEXTURE_SERVICE()
                ->createRenderTexture( image, textureArray.width, textureArray.height, MENGINE_DOCUMENT_FACTORABLE );

            if( texture == nullptr )
            {
                LOGGER_ERROR( "tiledmap2d '%s' can't create render texture for texture array"
                    , this->getName().c_str()
                );

                this->releaseBatches_();

                return false;
            }

            textureArray.texture = texture;
            m_textureArrays.emplace_back( texture );
            m_textureArrayLayerCount += hardwareLayerCount;

            for( uint32_t sourceIndex : textureArray.sourceIndices )
            {
                const TextureArraySource & source = textureArraySources[sourceIndex];

                if( this->uploadTextureArrayLayer_( image, source.textureArrayLayer, source.loader, source.desc, source.transparentColor ) == false )
                {
                    LOGGER_ERROR( "tiledmap2d '%s' can't upload texture array source '%s' to layer %u"
                        , this->getName().c_str()
                        , source.resourceName.c_str()
                        , source.textureArrayLayer
                    );

                    this->releaseBatches_();

                    return false;
                }
            }

            for( uint32_t layer = sourceLayerCount; layer != hardwareLayerCount; ++layer )
            {
                Rect rect;
                rect.left = 0;
                rect.top = 0;
                rect.right = image->getHWWidth();
                rect.bottom = image->getHWHeight();

                RenderImageLockedInterfacePtr locked = image->lock( layer, 0, rect, false );

                if( locked == nullptr )
                {
                    this->releaseBatches_();

                    return false;
                }

                size_t pitch;
                void * buffer = locked->getLockedBuffer( &pitch );

                if( buffer == nullptr )
                {
                    image->unlock( locked, layer, 0, false );
                    this->releaseBatches_();

                    return false;
                }

                std::memset( buffer, 0, pitch * image->getHWHeight() );

                if( image->unlock( locked, layer, 0, true ) == false )
                {
                    this->releaseBatches_();

                    return false;
                }
            }
        }

        for( BuildBatch & sourceBatch : buildBatches )
        {
            TiledBatch batch;
            batch.layerIndex = sourceBatch.layerIndex;
            batch.vertexCount = (uint32_t)sourceBatch.vertices.size();
            batch.indexCount = (uint32_t)sourceBatch.indices.size();
            const mt::vec3f & firstPosition = sourceBatch.vertices.front().position;
            mt::box2_reset( &batch.boundingBox, firstPosition.x, firstPosition.y );

            for( const RenderVertex2D & vertex : sourceBatch.vertices )
            {
                mt::box2_add_internal_point( &batch.boundingBox, vertex.position.x, vertex.position.y );
            }

            mt::box2_add_internal_point( &m_mapBoundingBox, batch.boundingBox.minimum );
            mt::box2_add_internal_point( &m_mapBoundingBox, batch.boundingBox.maximum );

            batch.vertexBuffer = RENDER_SYSTEM()->createVertexBuffer( sizeof( RenderVertex2D ), BT_STATIC, MENGINE_DOCUMENT_FACTORABLE );
            batch.indexBuffer = RENDER_SYSTEM()->createIndexBuffer( sizeof( RenderIndex ), BT_STATIC, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( batch.vertexBuffer, "tiledmap2d '%s' can't create Tiled vertex buffer", this->getName().c_str() );
            MENGINE_ASSERTION_MEMORY_PANIC( batch.indexBuffer, "tiledmap2d '%s' can't create Tiled index buffer", this->getName().c_str() );

            if( batch.vertexBuffer->resize( batch.vertexCount ) == false ||
                batch.vertexBuffer->draw( sourceBatch.vertices.data(), 0, batch.vertexCount ) == false ||
                batch.indexBuffer->resize( batch.indexCount ) == false ||
                batch.indexBuffer->draw( sourceBatch.indices.data(), 0, batch.indexCount ) == false )
            {
                this->releaseBatches_();

                return false;
            }

            const RenderTextureInterfacePtr & texture = textureArrayBuilds[sourceBatch.textureArrayIndex].texture;
            batch.material = RENDERMATERIAL_SERVICE()->getMaterial( m_materialName, PT_TRIANGLELIST, &texture, 1, MENGINE_DOCUMENT_FACTORABLE );

            if( batch.material == nullptr )
            {
                this->releaseBatches_();

                return false;
            }

            m_vertexCount += batch.vertexCount;
            m_indexCount += batch.indexCount;
            m_batches.emplace_back( std::move( batch ) );
        }

        if( m_batches.empty() == true )
        {
            mt::box2_reset( &m_mapBoundingBox, 0.f, 0.f );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TiledMap2D::uploadTextureArrayLayer_( const RenderImageInterfacePtr & _image, uint32_t _layer, const RenderImageLoaderInterfacePtr & _loader, const RenderImageDesc & _desc, const Optional<uint32_t> & _transparentColor ) const
    {
        MemoryInterfacePtr memory = _loader->getMemory( DF_IMAGE_NONE, MENGINE_DOCUMENT_FACTORABLE );

        if( memory == nullptr )
        {
            return false;
        }

        uint32_t channels = Helper::getPixelFormatChannels( _desc.format );
        size_t sourcePitch = (size_t)_desc.width * channels;
        size_t sourceSize = sourcePitch * _desc.height;

        if( channels == 0 || _desc.width == 0 || _desc.height == 0 || memory->getSize() < sourceSize )
        {
            return false;
        }

        uint32_t hwWidth = _image->getHWWidth();
        uint32_t hwHeight = _image->getHWHeight();

        if( _desc.width > hwWidth || _desc.height > hwHeight )
        {
            return false;
        }

        Rect rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = hwWidth;
        rect.bottom = hwHeight;

        RenderImageLockedInterfacePtr locked = _image->lock( _layer, 0, rect, false );

        if( locked == nullptr )
        {
            return false;
        }

        size_t destinationPitch;
        void * destinationBuffer = locked->getLockedBuffer( &destinationPitch );

        if( destinationBuffer == nullptr || destinationPitch < (size_t)hwWidth * channels )
        {
            _image->unlock( locked, _layer, 0, false );

            return false;
        }

        std::memset( destinationBuffer, 0, destinationPitch * hwHeight );

        const uint8_t * sourceBuffer = static_cast<const uint8_t *>(memory->getBuffer());
        uint8_t * destinationBytes = static_cast<uint8_t *>(destinationBuffer);

        uint8_t transparentRed = 0;
        uint8_t transparentGreen = 0;
        uint8_t transparentBlue = 0;

        if( _transparentColor.has_value() == true )
        {
            if( (_desc.format != PF_A8R8G8B8 && _desc.format != PF_X8R8G8B8) || channels != 4 )
            {
                _image->unlock( locked, _layer, 0, false );

                return false;
            }

            uint32_t transparentColor = _transparentColor.value();
            transparentRed = (uint8_t)(transparentColor >> 16);
            transparentGreen = (uint8_t)(transparentColor >> 8);
            transparentBlue = (uint8_t)transparentColor;
        }

#if defined(MENGINE_RENDER_TEXTURE_RGBA)
        const uint32_t redOffset = 0;
        const uint32_t greenOffset = 1;
        const uint32_t blueOffset = 2;
#else
        const uint32_t redOffset = 2;
        const uint32_t greenOffset = 1;
        const uint32_t blueOffset = 0;
#endif

        for( uint32_t y = 0; y != _desc.height; ++y )
        {
            uint8_t * destinationRow = destinationBytes + (size_t)y * destinationPitch;
            Helper::memoryCopy( destinationRow, 0, sourceBuffer, (size_t)y * sourcePitch, sourcePitch );

            if( _transparentColor.has_value() == true )
            {
                for( uint32_t x = 0; x != _desc.width; ++x )
                {
                    uint8_t * pixel = destinationRow + (size_t)x * channels;

                    if( pixel[redOffset] == transparentRed && pixel[greenOffset] == transparentGreen && pixel[blueOffset] == transparentBlue )
                    {
                        pixel[0] = 0;
                        pixel[1] = 0;
                        pixel[2] = 0;
                        pixel[3] = 0;
                    }
                }
            }

            for( uint32_t x = _desc.width; x != hwWidth; ++x )
            {
                Helper::memoryCopy( destinationRow, (size_t)x * channels, destinationRow, (size_t)(_desc.width - 1) * channels, channels );
            }
        }

        for( uint32_t y = _desc.height; y != hwHeight; ++y )
        {
            Helper::memoryCopy( destinationBytes, (size_t)y * destinationPitch, destinationBytes, (size_t)(_desc.height - 1) * destinationPitch, destinationPitch );
        }

        return _image->unlock( locked, _layer, 0, true );
    }
    //////////////////////////////////////////////////////////////////////////
    void TiledMap2D::_release()
    {
        this->releaseBatches_();
        m_tileLayerVisibilities.clear();

        if( m_resourceTiledMap != nullptr )
        {
            m_resourceTiledMap->release();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void TiledMap2D::_dispose()
    {
        m_resourceTiledMap = nullptr;
        m_tileLayerVisibilities.clear();

        Node::_dispose();
    }
    //////////////////////////////////////////////////////////////////////////
    void TiledMap2D::releaseBatches_()
    {
        for( TiledBatch & batch : m_batches )
        {
            batch.material = nullptr;
            batch.vertexBuffer = nullptr;
            batch.indexBuffer = nullptr;
        }

        m_batches.clear();
        m_textureArrays.clear();
        m_vertexCount = 0;
        m_indexCount = 0;
        m_textureArrayLayerCount = 0;
        mt::box2_reset( &m_mapBoundingBox, 0.f, 0.f );
    }
    //////////////////////////////////////////////////////////////////////////
    void TiledMap2D::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        const mt::mat4f & wm = this->getWorldMatrix();

        if( _context->transformation == nullptr )
        {
            m_renderWorldMatrix = wm;
        }
        else
        {
            const mt::mat4f & parentWM = _context->transformation->getTransformationWorldMatrix();
            m_renderWorldMatrix = wm * parentWM;
        }

        RenderContext newContext = *_context;
        newContext.transformation = this;
        const mt::mat4f & vpm = _context->camera->getCameraViewProjectionMatrix();
        const mt::vec2f screenMinimum( 0.f, 0.f );
        const mt::vec2f screenMaximum( 1.f, 1.f );

        for( const TiledBatch & batch : m_batches )
        {
            if( batch.layerIndex >= m_tileLayerVisibilities.size() || m_tileLayerVisibilities[batch.layerIndex] == false )
            {
                continue;
            }

            const mt::vec2f localCorners[4] = {
                mt::vec2f( batch.boundingBox.minimum.x, batch.boundingBox.minimum.y ),
                mt::vec2f( batch.boundingBox.maximum.x, batch.boundingBox.minimum.y ),
                mt::vec2f( batch.boundingBox.maximum.x, batch.boundingBox.maximum.y ),
                mt::vec2f( batch.boundingBox.minimum.x, batch.boundingBox.maximum.y )
            };

            mt::box2f screenBoundingBox;

            for( uint32_t index = 0; index != 4; ++index )
            {
                mt::vec2f worldPoint;
                mt::mul_v2_v2_m4( &worldPoint, localCorners[index], m_renderWorldMatrix );

                mt::vec2f projectedPoint;
                mt::mul_v2_v2_m4_homogenize( &projectedPoint, worldPoint, vpm );
                projectedPoint.x = (1.f + projectedPoint.x) * 0.5f;
                projectedPoint.y = (1.f - projectedPoint.y) * 0.5f;

                if( index == 0 )
                {
                    mt::box2_reset( &screenBoundingBox, projectedPoint.x, projectedPoint.y );
                }
                else
                {
                    mt::box2_add_internal_point( &screenBoundingBox, projectedPoint.x, projectedPoint.y );
                }
            }

            if( mt::box2_intersect( screenBoundingBox.minimum, screenBoundingBox.maximum, screenMinimum, screenMaximum ) == false )
            {
                continue;
            }

            _renderPipeline->addRenderMesh( &newContext
                , batch.material
                , nullptr
                , batch.vertexBuffer
                , batch.indexBuffer
                , batch.vertexCount
                , batch.indexCount
                , 0
                , 0
                , MENGINE_DOCUMENT_FORWARD );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
