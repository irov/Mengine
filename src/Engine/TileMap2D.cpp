#include "TileMap2D.h"

#include "Interface/RenderMaterialServiceInterface.h"
#include "Interface/RenderPipelineInterface.h"
#include "Interface/RenderSystemInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/RenderIndex.h"
#include "Kernel/RenderVertex2D.h"
#include "Kernel/VectorRenderIndex.h"
#include "Kernel/VectorRenderVertex2D.h"

#include "math/uv4.h"

#include <cstring>
#include <limits>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        static const uint32_t TILEMAP2D_VERTEX_PER_TILE = 4;
        static const uint32_t TILEMAP2D_INDEX_PER_TILE = 6;

        static const uint32_t TILEMAP2D_COEFF_X[TILEMAP2D_VERTEX_PER_TILE] = {0, 1, 1, 0};
        static const uint32_t TILEMAP2D_COEFF_Y[TILEMAP2D_VERTEX_PER_TILE] = {0, 0, 1, 1};
    }
    //////////////////////////////////////////////////////////////////////////
    TileMap2D::TileMap2D()
        : m_columns( 0 )
        , m_rows( 0 )
        , m_tileSize( 0.f )
        , m_vertexCount( 0 )
        , m_indexCount( 0 )
        , m_vertexBufferUploadCount( 0 )
        , m_indexBufferUploadCount( 0 )
        , m_materialName( STRINGIZE_STRING_LOCAL( "Texture_Solid" ) )
    {
        mt::ident_m4( &m_renderWorldMatrix );
    }
    //////////////////////////////////////////////////////////////////////////
    TileMap2D::~TileMap2D()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TileMap2D::setupTileMap( uint32_t _columns, uint32_t _rows, float _tileSize )
    {
        if( this->isCompile() == true )
        {
            LOGGER_ERROR( "tilemap2d '%s' setup is immutable after compile"
                , this->getName().c_str()
            );

            return false;
        }

        if( _columns == 0 || _rows == 0 || _tileSize <= 0.f )
        {
            LOGGER_ERROR( "tilemap2d '%s' invalid setup [%u:%u] tile size %.3f"
                , this->getName().c_str()
                , _columns
                , _rows
                , _tileSize
            );

            return false;
        }

        const uint64_t tileCount64 = (uint64_t)_columns * (uint64_t)_rows;
        const uint64_t vertexCount64 = tileCount64 * Detail::TILEMAP2D_VERTEX_PER_TILE;
        const uint64_t indexCount64 = tileCount64 * Detail::TILEMAP2D_INDEX_PER_TILE;

        if( vertexCount64 > (uint64_t)std::numeric_limits<RenderIndex>::max() ||
            vertexCount64 > (uint64_t)std::numeric_limits<uint32_t>::max() ||
            indexCount64 > (uint64_t)std::numeric_limits<uint32_t>::max() )
        {
            LOGGER_ERROR( "tilemap2d '%s' setup exceeds uint16 index range [%u:%u]"
                , this->getName().c_str()
                , _columns
                , _rows
            );

            return false;
        }

        m_columns = _columns;
        m_rows = _rows;
        m_tileSize = _tileSize;
        m_vertexCount = (uint32_t)vertexCount64;
        m_indexCount = (uint32_t)indexCount64;

        m_tiles.clear();
        m_tiles.resize( (VectorTileMap2DTiles::size_type)tileCount64 );
        m_compiledVertices.clear();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TileMap2D::setTileResource( uint32_t _column, uint32_t _row, const ResourceImagePtr & _resource )
    {
        if( this->isCompile() == true )
        {
            LOGGER_ERROR( "tilemap2d '%s' resources are immutable after compile; streaming must use setTileActive"
                , this->getName().c_str()
            );

            return false;
        }

        if( _resource == nullptr || _column >= m_columns || _row >= m_rows )
        {
            return false;
        }

        TileMap2DTile & tile = m_tiles[this->getTileIndex_( _column, _row )];
        tile.resource = _resource;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceImagePtr & TileMap2D::getTileResource( uint32_t _column, uint32_t _row ) const
    {
        if( _column >= m_columns || _row >= m_rows )
        {
            return ResourceImagePtr::none();
        }

        return m_tiles[this->getTileIndex_( _column, _row )].resource;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TileMap2D::clearTileResource( uint32_t _column, uint32_t _row )
    {
        if( this->isCompile() == true || _column >= m_columns || _row >= m_rows )
        {
            return false;
        }

        TileMap2DTile & tile = m_tiles[this->getTileIndex_( _column, _row )];
        tile.resource = nullptr;
        tile.active = false;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TileMap2D::setTileActive( uint32_t _column, uint32_t _row, bool _active )
    {
        if( _column >= m_columns || _row >= m_rows )
        {
            return false;
        }

        TileMap2DTile & tile = m_tiles[this->getTileIndex_( _column, _row )];

        if( tile.resource == nullptr )
        {
            return false;
        }

        if( tile.active == _active )
        {
            return true;
        }

        if( this->isCompile() == false )
        {
            tile.active = _active;

            return true;
        }

        if( _active == true )
        {
            if( this->compileTile_( &tile ) == false )
            {
                return false;
            }
        }
        else
        {
            this->releaseTile_( &tile );
        }

        tile.active = _active;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TileMap2D::isTileActive( uint32_t _column, uint32_t _row ) const
    {
        if( _column >= m_columns || _row >= m_rows )
        {
            return false;
        }

        return m_tiles[this->getTileIndex_( _column, _row )].active;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TileMap2D::isTileResident( uint32_t _column, uint32_t _row ) const
    {
        if( _column >= m_columns || _row >= m_rows )
        {
            return false;
        }

        return m_tiles[this->getTileIndex_( _column, _row )].resident;
    }
    //////////////////////////////////////////////////////////////////////////
    void TileMap2D::setMaterialName( const ConstString & _materialName )
    {
        if( this->isCompile() == true )
        {
            LOGGER_ERROR( "tilemap2d '%s' material is immutable after compile"
                , this->getName().c_str()
            );

            return;
        }

        m_materialName = _materialName;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & TileMap2D::getMaterialName() const
    {
        return m_materialName;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TileMap2D::getColumnCount() const
    {
        return m_columns;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TileMap2D::getRowCount() const
    {
        return m_rows;
    }
    //////////////////////////////////////////////////////////////////////////
    float TileMap2D::getTileSize() const
    {
        return m_tileSize;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TileMap2D::getTileCount() const
    {
        return (uint32_t)m_tiles.size();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TileMap2D::getResidentTileCount() const
    {
        uint32_t count = 0;

        for( const TileMap2DTile & tile : m_tiles )
        {
            if( tile.resident == true )
            {
                ++count;
            }
        }

        return count;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TileMap2D::getVertexCount() const
    {
        return m_vertexCount;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TileMap2D::getIndexCount() const
    {
        return m_indexCount;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TileMap2D::getVertexBufferUploadCount() const
    {
        return m_vertexBufferUploadCount;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TileMap2D::getIndexBufferUploadCount() const
    {
        return m_indexBufferUploadCount;
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t TileMap2D::getResidentTextureMemoryBytes() const
    {
        uint64_t bytes = 0;

        for( const TileMap2DTile & tile : m_tiles )
        {
            if( tile.resident == false )
            {
                continue;
            }

            const mt::vec2f & size = tile.resource->getMaxSize();
            bytes += (uint64_t)size.x * (uint64_t)size.y * 4ULL;
        }

        return bytes;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TileMap2D::validateSeams() const
    {
        if( m_columns == 0 || m_rows == 0 || m_tileSize <= 0.f || m_compiledVertices.size() != m_vertexCount )
        {
            return false;
        }

        const auto equalPosition = []( const mt::vec3f & _left, const mt::vec3f & _right )
        {
            return std::memcmp( &_left.x, &_right.x, sizeof( float ) ) == 0 &&
                std::memcmp( &_left.y, &_right.y, sizeof( float ) ) == 0 &&
                std::memcmp( &_left.z, &_right.z, sizeof( float ) ) == 0;
        };

        for( uint32_t row = 0; row != m_rows; ++row )
        {
            for( uint32_t column = 0; column + 1 < m_columns; ++column )
            {
                const uint32_t leftOffset = this->getTileIndex_( column, row ) * Detail::TILEMAP2D_VERTEX_PER_TILE;
                const uint32_t rightOffset = this->getTileIndex_( column + 1, row ) * Detail::TILEMAP2D_VERTEX_PER_TILE;

                if( equalPosition( m_compiledVertices[leftOffset + 1].position, m_compiledVertices[rightOffset + 0].position ) == false ||
                    equalPosition( m_compiledVertices[leftOffset + 2].position, m_compiledVertices[rightOffset + 3].position ) == false )
                {
                    return false;
                }
            }
        }

        for( uint32_t column = 0; column != m_columns; ++column )
        {
            for( uint32_t row = 0; row + 1 < m_rows; ++row )
            {
                const uint32_t topOffset = this->getTileIndex_( column, row ) * Detail::TILEMAP2D_VERTEX_PER_TILE;
                const uint32_t bottomOffset = this->getTileIndex_( column, row + 1 ) * Detail::TILEMAP2D_VERTEX_PER_TILE;

                if( equalPosition( m_compiledVertices[topOffset + 3].position, m_compiledVertices[bottomOffset + 0].position ) == false ||
                    equalPosition( m_compiledVertices[topOffset + 2].position, m_compiledVertices[bottomOffset + 1].position ) == false )
                {
                    return false;
                }
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::mat4f & TileMap2D::getTransformationWorldMatrix() const
    {
        return m_renderWorldMatrix;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TileMap2D::_compile()
    {
        if( m_columns == 0 || m_rows == 0 || m_tiles.empty() == true )
        {
            LOGGER_ERROR( "tilemap2d '%s' is not setup"
                , this->getName().c_str()
            );

            return false;
        }

        m_compiledVertices.clear();
        m_compiledVertices.resize( m_vertexCount );
        VectorRenderIndex indices( m_indexCount );
        Vector<float> gridX( m_columns + 1 );
        Vector<float> gridY( m_rows + 1 );

        for( uint32_t column = 0; column != m_columns + 1; ++column )
        {
            gridX[column] = (float)column * m_tileSize;
        }

        for( uint32_t row = 0; row != m_rows + 1; ++row )
        {
            gridY[row] = (float)row * m_tileSize;
        }

        for( uint32_t row = 0; row != m_rows; ++row )
        {
            for( uint32_t column = 0; column != m_columns; ++column )
            {
                const uint32_t tileIndex = this->getTileIndex_( column, row );
                const uint32_t vertexOffset = tileIndex * Detail::TILEMAP2D_VERTEX_PER_TILE;
                const uint32_t indexOffset = tileIndex * Detail::TILEMAP2D_INDEX_PER_TILE;

                mt::uv4f uvImage;
                mt::uv4_identity( &uvImage );

                const TileMap2DTile & tile = m_tiles[tileIndex];

                if( tile.resource != nullptr )
                {
                    uvImage = tile.resource->getUVTexture( 0 );
                }

                for( uint32_t vertexIndex = 0; vertexIndex != Detail::TILEMAP2D_VERTEX_PER_TILE; ++vertexIndex )
                {
                    RenderVertex2D & vertex = m_compiledVertices[vertexOffset + vertexIndex];
                    vertex.position.x = gridX[column + Detail::TILEMAP2D_COEFF_X[vertexIndex]];
                    vertex.position.y = gridY[row + Detail::TILEMAP2D_COEFF_Y[vertexIndex]];
                    vertex.position.z = 0.f;
                    vertex.color = 0xFFFFFFFF;
                    vertex.uv[0] = uvImage[vertexIndex];
                    vertex.uv[1] = uvImage[vertexIndex];
                }

                indices[indexOffset + 0] = (RenderIndex)(vertexOffset + 0);
                indices[indexOffset + 1] = (RenderIndex)(vertexOffset + 1);
                indices[indexOffset + 2] = (RenderIndex)(vertexOffset + 2);
                indices[indexOffset + 3] = (RenderIndex)(vertexOffset + 0);
                indices[indexOffset + 4] = (RenderIndex)(vertexOffset + 2);
                indices[indexOffset + 5] = (RenderIndex)(vertexOffset + 3);
            }
        }

        m_vertexBuffer = RENDER_SYSTEM()
            ->createVertexBuffer( sizeof( RenderVertex2D ), BT_STATIC, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( m_vertexBuffer, "tilemap2d '%s' failed to create vertex buffer"
            , this->getName().c_str()
        );

        if( m_vertexBuffer->resize( m_vertexCount ) == false ||
            m_vertexBuffer->draw( m_compiledVertices.data(), 0, m_vertexCount ) == false )
        {
            LOGGER_ERROR( "tilemap2d '%s' failed to upload %u vertices"
                , this->getName().c_str()
                , m_vertexCount
            );

            this->releaseCompiledData_();

            return false;
        }

        ++m_vertexBufferUploadCount;

        m_indexBuffer = RENDER_SYSTEM()
            ->createIndexBuffer( sizeof( RenderIndex ), BT_STATIC, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( m_indexBuffer, "tilemap2d '%s' failed to create index buffer"
            , this->getName().c_str()
        );

        if( m_indexBuffer->resize( m_indexCount ) == false ||
            m_indexBuffer->draw( indices.data(), 0, m_indexCount ) == false )
        {
            LOGGER_ERROR( "tilemap2d '%s' failed to upload %u indices"
                , this->getName().c_str()
                , m_indexCount
            );

            this->releaseCompiledData_();

            return false;
        }

        ++m_indexBufferUploadCount;

        for( TileMap2DTile & tile : m_tiles )
        {
            if( tile.active == false )
            {
                continue;
            }

            if( this->compileTile_( &tile ) == false )
            {
                this->releaseCompiledData_();

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TileMap2D::_release()
    {
        this->releaseCompiledData_();
    }
    //////////////////////////////////////////////////////////////////////////
    void TileMap2D::_dispose()
    {
        m_tiles.clear();
        m_compiledVertices.clear();

        Node::_dispose();
    }
    //////////////////////////////////////////////////////////////////////////
    void TileMap2D::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_vertexBuffer, "tilemap2d '%s' render without vertex buffer"
            , this->getName().c_str()
        );
        MENGINE_ASSERTION_MEMORY_PANIC( m_indexBuffer, "tilemap2d '%s' render without index buffer"
            , this->getName().c_str()
        );

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

        for( uint32_t tileIndex = 0; tileIndex != (uint32_t)m_tiles.size(); ++tileIndex )
        {
            const TileMap2DTile & tile = m_tiles[tileIndex];

            if( tile.resident == false )
            {
                continue;
            }

            _renderPipeline->addRenderMesh( &newContext
                , tile.material
                , nullptr
                , m_vertexBuffer
                , m_indexBuffer
                , m_vertexCount
                , Detail::TILEMAP2D_INDEX_PER_TILE
                , 0
                , tileIndex * Detail::TILEMAP2D_INDEX_PER_TILE
                , MENGINE_DOCUMENT_FORWARD );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TileMap2D::getTileIndex_( uint32_t _column, uint32_t _row ) const
    {
        return _row * m_columns + _column;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TileMap2D::compileTile_( TileMap2DTile * const _tile )
    {
        if( _tile->resident == true )
        {
            return true;
        }

        if( _tile->resource == nullptr || _tile->resource->compile() == false )
        {
            LOGGER_ERROR( "tilemap2d '%s' failed to compile tile resource"
                , this->getName().c_str()
            );

            return false;
        }

        const RenderTextureInterfacePtr & texture = _tile->resource->getTexture( 0 );

        _tile->material = RENDERMATERIAL_SERVICE()
            ->getMaterial( m_materialName, PT_TRIANGLELIST, &texture, 1, MENGINE_DOCUMENT_FACTORABLE );

        if( _tile->material == nullptr )
        {
            LOGGER_ERROR( "tilemap2d '%s' failed to create opaque material '%s' for resource '%s'"
                , this->getName().c_str()
                , m_materialName.c_str()
                , _tile->resource->getName().c_str()
            );

            _tile->resource->release();

            return false;
        }

        _tile->resident = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TileMap2D::releaseTile_( TileMap2DTile * const _tile )
    {
        if( _tile->resident == false )
        {
            return;
        }

        _tile->material = nullptr;
        _tile->resource->release();
        _tile->resident = false;
    }
    //////////////////////////////////////////////////////////////////////////
    void TileMap2D::releaseCompiledData_()
    {
        for( TileMap2DTile & tile : m_tiles )
        {
            this->releaseTile_( &tile );
        }

        m_vertexBuffer = nullptr;
        m_indexBuffer = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}
