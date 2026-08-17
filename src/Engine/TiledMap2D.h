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
#include "Kernel/String.h"
#include "Kernel/Vector.h"

#include "Engine/ResourceTiledMap.h"

#include "math/box2.h"
#include "math/mat4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TiledMap2D
        : public Node
        , public RenderTransformationInterface
        , protected BaseRender
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( TiledMap2D );
        DECLARE_VISITABLE( Node );
        DECLARE_RENDERABLE();
        DECLARE_TRANSFORMABLE();

    public:
        TiledMap2D();
        ~TiledMap2D() override;

    public:
        void setResourceTiledMap( const ResourceTiledMapPtr & _resource );
        const ResourceTiledMapPtr & getResourceTiledMap() const;

        void setMaterialName( const ConstString & _materialName );
        const ConstString & getMaterialName() const;

        ETiledMapOrientation getOrientation() const;
        uint32_t getColumnCount() const;
        uint32_t getRowCount() const;
        uint32_t getTileWidth() const;
        uint32_t getTileHeight() const;
        uint32_t getTileCount() const;
        uint32_t getTileLayerCount() const;
        uint32_t getObjectLayerCount() const;
        const String & getTileLayerName( uint32_t _index ) const;
        const String & getObjectLayerName( uint32_t _index ) const;
        bool setTileLayerVisible( uint32_t _index, bool _visible );
        bool isTileLayerVisible( uint32_t _index ) const;
        uint32_t getObjectCount( uint32_t _layerIndex ) const;
        uint32_t getObjectId( uint32_t _layerIndex, uint32_t _objectIndex ) const;
        const String & getObjectName( uint32_t _layerIndex, uint32_t _objectIndex ) const;
        const String & getObjectType( uint32_t _layerIndex, uint32_t _objectIndex ) const;
        const String & getObjectClass( uint32_t _layerIndex, uint32_t _objectIndex ) const;
        ETiledMapObjectShape getObjectShape( uint32_t _layerIndex, uint32_t _objectIndex ) const;
        mt::vec2f getObjectPosition( uint32_t _layerIndex, uint32_t _objectIndex ) const;
        mt::vec2f getObjectSize( uint32_t _layerIndex, uint32_t _objectIndex ) const;
        float getObjectRotation( uint32_t _layerIndex, uint32_t _objectIndex ) const;
        uint32_t getObjectPointCount( uint32_t _layerIndex, uint32_t _objectIndex ) const;
        mt::vec2f getObjectPoint( uint32_t _layerIndex, uint32_t _objectIndex, uint32_t _pointIndex ) const;
        TiledMapProperties getMapProperties() const;
        TiledMapProperties getTileLayerProperties( uint32_t _layerIndex ) const;
        TiledMapProperties getObjectLayerProperties( uint32_t _layerIndex ) const;
        TiledMapProperties getObjectProperties( uint32_t _layerIndex, uint32_t _objectIndex ) const;
        uint32_t getBatchCount() const;
        uint32_t getVertexCount() const;
        uint32_t getIndexCount() const;
        uint64_t getResidentTextureMemoryBytes() const;

    public:
        const mt::mat4f & getTransformationWorldMatrix() const override;

    protected:
        bool _compile() override;
        void _release() override;
        void _dispose() override;

    protected:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    protected:
        bool compileBatches_();
        void releaseBatches_();

    protected:
        typedef Vector<uint8_t> VectorTiledMapLayerVisibilities;

        struct TiledBatch
        {
            uint32_t layerIndex;
            String resourceName;
            ResourceImagePtr resource;
            RenderMaterialInterfacePtr material;
            RenderVertexBufferInterfacePtr vertexBuffer;
            RenderIndexBufferInterfacePtr indexBuffer;
            uint32_t vertexCount;
            uint32_t indexCount;
            mt::box2f boundingBox;
        };
        typedef Vector<TiledBatch> VectorTiledBatches;

    protected:
        ResourceTiledMapPtr m_resourceTiledMap;
        VectorTiledMapLayerVisibilities m_tileLayerVisibilities;
        VectorTiledBatches m_batches;
        uint32_t m_vertexCount;
        uint32_t m_indexCount;

        ConstString m_materialName;

        mutable mt::mat4f m_renderWorldMatrix;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<TiledMap2D> TiledMap2DPtr;
    //////////////////////////////////////////////////////////////////////////
}
