#include "TiledMap2D.h"

#include "Interface/RenderMaterialServiceInterface.h"
#include "Interface/RenderPipelineInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/RenderCameraInterface.h"
#include "Interface/ResourceServiceInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/ResourceCast.h"
#include "Kernel/VectorRenderIndex.h"
#include "Kernel/VectorRenderVertex2D.h"

#include <cmath>
#include <limits>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TiledMap2D::TiledMap2D()
        : m_vertexCount( 0 )
        , m_indexCount( 0 )
        , m_materialName( STRINGIZE_STRING_LOCAL( "Texture_Solid" ) )
    {
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
    uint64_t TiledMap2D::getResidentTextureMemoryBytes() const
    {
        uint64_t bytes = 0;

        for( VectorTiledBatches::size_type index = 0; index != m_batches.size(); ++index )
        {
            const TiledBatch & batch = m_batches[index];
            bool alreadyCounted = false;

            for( VectorTiledBatches::size_type previousIndex = 0; previousIndex != index; ++previousIndex )
            {
                if( m_batches[previousIndex].resource == batch.resource )
                {
                    alreadyCounted = true;

                    break;
                }
            }

            if( alreadyCounted == true )
            {
                continue;
            }

            const mt::vec2f & size = batch.resource->getMaxSize();
            bytes += (uint64_t)size.x * (uint64_t)size.y * 4ULL;
        }

        return bytes;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::mat4f & TiledMap2D::getTransformationWorldMatrix() const
    {
        return m_renderWorldMatrix;
    }
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

        struct BuildBatch
        {
            uint32_t layerIndex;
            int32_t chunkX;
            int32_t chunkY;
            String resourceName;
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

                if( tileset == nullptr || gid - tileset->firstGid >= tileset->tileCount )
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
                uint32_t imageWidth = tileset->imageWidth;
                uint32_t imageHeight = tileset->imageHeight;
                uint32_t sourceX = tileset->margin;
                uint32_t sourceY = tileset->margin;
                uint32_t sourceWidth = tileset->tileWidth;
                uint32_t sourceHeight = tileset->tileHeight;

                if( tileset->columns != 0 )
                {
                    sourceX += localId % tileset->columns * (tileset->tileWidth + tileset->spacing);
                    sourceY += localId / tileset->columns * (tileset->tileHeight + tileset->spacing);
                }

                for( const TiledMapTileImage & image : tileset->tileImages )
                {
                    if( image.localId == localId )
                    {
                        resourceName = image.resourceName;
                        imageWidth = image.imageWidth;
                        imageHeight = image.imageHeight;
                        sourceX = 0;
                        sourceY = 0;
                        sourceWidth = image.imageWidth;
                        sourceHeight = image.imageHeight;
                        break;
                    }
                }

                if( resourceName.empty() == true || imageWidth == 0 || imageHeight == 0 || sourceWidth == 0 || sourceHeight == 0 )
                {
                    LOGGER_ERROR( "tiledmap2d '%s' tile gid %u has invalid image metadata"
                        , this->getName().c_str()
                        , gid
                    );

                    this->releaseBatches_();

                    return false;
                }

                BuildBatch * batch = nullptr;
                int32_t chunkX = (int32_t)std::floor( (float)tile.x / (float)chunkSize );
                int32_t chunkY = (int32_t)std::floor( (float)tile.y / (float)chunkSize );

                for( BuildBatch & candidate : buildBatches )
                {
                    if( candidate.layerIndex == layerIndex &&
                        candidate.chunkX == chunkX &&
                        candidate.chunkY == chunkY &&
                        candidate.resourceName == resourceName &&
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
                    batch->resourceName = resourceName;
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

                    vertex.uv[0] = mt::vec2f(
                        ((float)sourceX + sourceCornerX * (float)sourceWidth) / (float)imageWidth,
                        ((float)sourceY + sourceCornerY * (float)sourceHeight) / (float)imageHeight
                    );
                    vertex.uv[1] = vertex.uv[0];
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
        m_batches.reserve( buildBatches.size() );
        const ConstString & groupName = m_resourceTiledMap->getGroupName();

        for( BuildBatch & sourceBatch : buildBatches )
        {
            ResourcePtr baseResource = RESOURCE_SERVICE()
                ->getResourceReference( groupName, Helper::stringizeString( sourceBatch.resourceName ) );

            ResourceImagePtr resource = Helper::dynamicResourceCast<ResourceImagePtr>( baseResource );

            if( resource == nullptr || resource->compile() == false )
            {
                LOGGER_ERROR( "tiledmap2d '%s' can't compile Tiled image resource '%s'"
                    , this->getName().c_str()
                    , sourceBatch.resourceName.c_str()
                );

                this->releaseBatches_();

                return false;
            }

            const mt::uv4f & resourceUV = resource->getUVTexture( 0 );

            for( RenderVertex2D & vertex : sourceBatch.vertices )
            {
                const mt::vec2f normalizedUV = vertex.uv[0];
                mt::vec2f top;
                top.x = resourceUV[0].x + (resourceUV[1].x - resourceUV[0].x) * normalizedUV.x;
                top.y = resourceUV[0].y + (resourceUV[1].y - resourceUV[0].y) * normalizedUV.x;
                mt::vec2f bottom;
                bottom.x = resourceUV[3].x + (resourceUV[2].x - resourceUV[3].x) * normalizedUV.x;
                bottom.y = resourceUV[3].y + (resourceUV[2].y - resourceUV[3].y) * normalizedUV.x;
                vertex.uv[0].x = top.x + (bottom.x - top.x) * normalizedUV.y;
                vertex.uv[0].y = top.y + (bottom.y - top.y) * normalizedUV.y;
                vertex.uv[1] = vertex.uv[0];
            }

            TiledBatch batch;
            batch.layerIndex = sourceBatch.layerIndex;
            batch.resourceName = sourceBatch.resourceName;
            batch.resource = resource;
            batch.vertexCount = (uint32_t)sourceBatch.vertices.size();
            batch.indexCount = (uint32_t)sourceBatch.indices.size();
            const mt::vec3f & firstPosition = sourceBatch.vertices.front().position;
            mt::box2_reset( &batch.boundingBox, firstPosition.x, firstPosition.y );

            for( const RenderVertex2D & vertex : sourceBatch.vertices )
            {
                mt::box2_add_internal_point( &batch.boundingBox, vertex.position.x, vertex.position.y );
            }

            batch.vertexBuffer = RENDER_SYSTEM()->createVertexBuffer( sizeof( RenderVertex2D ), BT_STATIC, MENGINE_DOCUMENT_FACTORABLE );
            batch.indexBuffer = RENDER_SYSTEM()->createIndexBuffer( sizeof( RenderIndex ), BT_STATIC, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( batch.vertexBuffer, "tiledmap2d '%s' can't create Tiled vertex buffer", this->getName().c_str() );
            MENGINE_ASSERTION_MEMORY_PANIC( batch.indexBuffer, "tiledmap2d '%s' can't create Tiled index buffer", this->getName().c_str() );

            if( batch.vertexBuffer->resize( batch.vertexCount ) == false ||
                batch.vertexBuffer->draw( sourceBatch.vertices.data(), 0, batch.vertexCount ) == false ||
                batch.indexBuffer->resize( batch.indexCount ) == false ||
                batch.indexBuffer->draw( sourceBatch.indices.data(), 0, batch.indexCount ) == false )
            {
                resource->release();
                this->releaseBatches_();

                return false;
            }

            const RenderTextureInterfacePtr & texture = resource->getTexture( 0 );
            batch.material = RENDERMATERIAL_SERVICE()->getMaterial( m_materialName, PT_TRIANGLELIST, &texture, 1, MENGINE_DOCUMENT_FACTORABLE );

            if( batch.material == nullptr )
            {
                resource->release();
                this->releaseBatches_();

                return false;
            }

            m_vertexCount += batch.vertexCount;
            m_indexCount += batch.indexCount;
            m_batches.emplace_back( std::move( batch ) );
        }

        return true;
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

            if( batch.resource != nullptr )
            {
                batch.resource->release();
                batch.resource = nullptr;
            }
        }

        m_batches.clear();
        m_vertexCount = 0;
        m_indexCount = 0;
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
