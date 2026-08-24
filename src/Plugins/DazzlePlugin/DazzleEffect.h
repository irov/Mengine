#pragma once

#include "DazzleInterface.h"
#include "DazzleIncluder.h"

#include "ResourceDazzleEffect.h"

#include "Kernel/Node.h"
#include "Kernel/BaseEventation.h"
#include "Kernel/BaseAnimation.h"
#include "Kernel/BaseUpdation.h"
#include "Kernel/BaseRender.h"
#include "Kernel/BaseTransformation.h"
#include "Kernel/Materialable.h"
#include "Kernel/Vector.h"

#include "math/vec2.h"
#include "math/vec4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<mt::vec2f> VectorDazzlePathPositions;
    typedef Vector<mt::vec4f> VectorDazzlePathColors;
    //////////////////////////////////////////////////////////////////////////
    class DazzleEffect
        : public Node
        , public Materialable
        , public UnknownDazzleEffectInterface
        , protected BaseEventation
        , protected BaseUpdation
        , protected BaseRender
        , protected BaseAnimation
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( DazzleEffect );
        DECLARE_VISITABLE( Node );
        DECLARE_UNKNOWABLE();
        DECLARE_ANIMATABLE();
        DECLARE_UPDATABLE();
        DECLARE_RENDERABLE();
        DECLARE_EVENTABLE();
        DECLARE_TRANSFORMABLE();

    private:
        typedef Vector<dz_path_point_t> VectorDazzlePathPoints;
        typedef Vector<RenderMaterialInterfacePtr> VectorDazzleRenderMaterials;

    public:
        DazzleEffect();
        ~DazzleEffect() override;

    public:
        void setDazzleService( const dz_service_t * _service );
        const dz_service_t * getDazzleService() const;

    public:
        void setResourceDazzle( const ResourcePtr & _resourceDazzleEffect ) override;
        const ResourcePtr & getResourceDazzle() const override;

        void setResourceImage( const ResourceImagePtr & _resource ) override;
        const ResourceImagePtr & getResourceImage() const override;

        bool setPathPoints( const VectorDazzlePathPositions & _points );
        bool setPathPointsColored( const VectorDazzlePathPositions & _points, const VectorDazzlePathColors & _colors );
        bool setPathPointsColoredGradient( const VectorDazzlePathPositions & _points, const VectorDazzlePathColors & _colors, float _scaleBegin, float _scaleEnd, float _alphaBegin, float _alphaEnd );
        void clearPathPoints();
        bool primePath( float _time );
        bool restartEffect( float _prewarm );

    protected:
        bool setPathPoints_( const VectorDazzlePathPositions & _points, float _scaleBegin, float _scaleEnd, float _alphaBegin, float _alphaEnd );
        bool setPathPointsColored_( const VectorDazzlePathPositions & _points, const VectorDazzlePathColors & _colors, float _scaleBegin, float _scaleEnd, float _alphaBegin, float _alphaEnd );
        bool makePathPoints_( const VectorDazzlePathPositions & _points, float _scaleBegin, float _scaleEnd, float _alphaBegin, float _alphaEnd, VectorDazzlePathPoints * const _pathPoints ) const;
        bool applyPathPointColors_( const VectorDazzlePathPositions & _points, const VectorDazzlePathColors & _colors, VectorDazzlePathPoints * const _pathPoints ) const;
        bool applyPathPoints_( const VectorDazzlePathPoints & _pathPoints );

    public:
        bool _play( uint32_t _enumerator, float _time ) override;
        bool _restart( uint32_t _enumerator, float _time ) override;
        void _pause( uint32_t _enumerator ) override;
        void _resume( uint32_t _enumerator, float _time ) override;
        bool _stop( uint32_t _enumerator ) override;
        void _end( uint32_t _enumerator ) override;
        bool _interrupt( uint32_t _enumerator ) override;

    protected:
        RenderMaterialInterfacePtr _updateMaterial() const override;

    protected:
        float _getDuration() const override;

    protected:
        void _setLoop( bool _value ) override;

    protected:
        bool _activate() override;
        void _deactivate() override;

        bool _compile() override;
        void _release() override;

    protected:
        void _dispose() override;

    protected:
        void update( const UpdateContext * _context ) override;
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

        void _updateBoundingBox( mt::box2f * const _boundingBox, mt::box2f ** const _boundingBoxCurrent ) const override;

    protected:
      bool makeCameraState_( const RenderContext * _context, dz_camera_state_t * const _camera ) const;
      RenderMaterialInterfacePtr makeMaterialPass_( const dz_material_pass_desc_t & _pass, dz_primitive_type_e _primitive ) const;
      uint32_t getRenderIndex_( dz_index_type_e _indexType, uint32_t _index ) const;

      void updateVertexColor_( RenderVertex2D * const _vertices, uint32_t _verticesCount ) const;
      void updateVertexWM_( RenderVertex2D * const _vertices, uint32_t _verticesCount ) const;
      void updateVertexUV_( RenderVertex2D * const _vertices, uint32_t _verticesCount ) const;

    protected:
        ResourcePtr m_resourceDazzleEffect;
        ResourceImagePtr m_resourceImage;

        const dz_service_t * m_service;

        dz_instance_t * m_instance;

        mutable RenderVertex2D * m_renderVertices;
        mutable dz_vec3_t * m_renderNormals;
        mutable dz_vec4_t * m_renderTangents;
        mutable dz_vec4_t * m_renderColors;
        mutable uint32_t m_renderVertexCount;

        mutable uint8_t * m_dazzleIndices;
        mutable size_t m_dazzleIndexSize;

        mutable dz_render_chunk_t * m_renderChunks;
        mutable uint32_t m_renderChunkCount;

        mutable RenderVertex2D * m_submitVertices;
        mutable RenderIndex * m_renderIndicies;
        mutable uint32_t m_renderIndexCount;

        mutable VectorDazzleRenderMaterials m_renderMaterials;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DazzleEffect, Node> DazzleEffectPtr;
    //////////////////////////////////////////////////////////////////////////
}
