#pragma once

#include "Interface/RenderIndexBufferInterface.h"
#include "Interface/RenderMaterialInterface.h"
#include "Interface/RenderTransformationInterface.h"
#include "Interface/RenderVertexBufferInterface.h"

#include "Kernel/BaseRender.h"
#include "Kernel/BaseTransformation.h"
#include "Kernel/ConstString.h"
#include "Kernel/Node.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/RenderVertex2D.h"
#include "Kernel/Vector.h"

#include "math/mat4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct TileMap2DTile
    {
        ResourceImagePtr resource;
        RenderMaterialInterfacePtr material;
        bool active = false;
        bool resident = false;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<TileMap2DTile> VectorTileMap2DTiles;
    //////////////////////////////////////////////////////////////////////////
    class TileMap2D
        : public Node
        , public RenderTransformationInterface
        , protected BaseRender
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( TileMap2D );
        DECLARE_VISITABLE( Node );
        DECLARE_RENDERABLE();
        DECLARE_TRANSFORMABLE();

    public:
        TileMap2D();
        ~TileMap2D() override;

    public:
        bool setupTileMap( uint32_t _columns, uint32_t _rows, float _tileSize );

        bool setTileResource( uint32_t _column, uint32_t _row, const ResourceImagePtr & _resource );
        const ResourceImagePtr & getTileResource( uint32_t _column, uint32_t _row ) const;
        bool clearTileResource( uint32_t _column, uint32_t _row );

        bool setTileActive( uint32_t _column, uint32_t _row, bool _active );
        bool isTileActive( uint32_t _column, uint32_t _row ) const;
        bool isTileResident( uint32_t _column, uint32_t _row ) const;

        void setMaterialName( const ConstString & _materialName );
        const ConstString & getMaterialName() const;

        uint32_t getColumnCount() const;
        uint32_t getRowCount() const;
        float getTileSize() const;
        uint32_t getTileCount() const;
        uint32_t getResidentTileCount() const;
        uint32_t getVertexCount() const;
        uint32_t getIndexCount() const;
        uint32_t getVertexBufferUploadCount() const;
        uint32_t getIndexBufferUploadCount() const;
        uint64_t getResidentTextureMemoryBytes() const;
        bool validateSeams() const;

    public:
        const mt::mat4f & getTransformationWorldMatrix() const override;

    protected:
        bool _compile() override;
        void _release() override;
        void _dispose() override;

    protected:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    protected:
        uint32_t getTileIndex_( uint32_t _column, uint32_t _row ) const;
        bool compileTile_( TileMap2DTile * const _tile );
        void releaseTile_( TileMap2DTile * const _tile );
        void releaseCompiledData_();

    protected:
        VectorTileMap2DTiles m_tiles;

        uint32_t m_columns;
        uint32_t m_rows;
        float m_tileSize;

        uint32_t m_vertexCount;
        uint32_t m_indexCount;
        uint32_t m_vertexBufferUploadCount;
        uint32_t m_indexBufferUploadCount;

        ConstString m_materialName;

        RenderVertexBufferInterfacePtr m_vertexBuffer;
        RenderIndexBufferInterfacePtr m_indexBuffer;
        Vector<RenderVertex2D> m_compiledVertices;

        mutable mt::mat4f m_renderWorldMatrix;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<TileMap2D> TileMap2DPtr;
    //////////////////////////////////////////////////////////////////////////
}
