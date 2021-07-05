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

namespace Mengine
{
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
        void updateVertexColor_( RenderVertex2D * const _vertices, uint16_t _verticesCount ) const;
        void updateVertexWM_( RenderVertex2D * const _vertices, uint16_t _verticesCount ) const;
        void updateVertexUV_( RenderVertex2D * const _vertices, uint16_t _verticesCount ) const;

    protected:
        ResourcePtr m_resourceDazzleEffect;
        ResourceImagePtr m_resourceImage;

        const dz_service_t * m_service;

        dz_instance_t * m_instance;

        mutable RenderVertex2D * m_renderVertices;
        mutable uint16_t m_renderVertexCount;

        mutable RenderIndex * m_renderIndicies;
        mutable uint16_t m_renderIndexCount;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DazzleEffect, Node> DazzleEffectPtr;
    //////////////////////////////////////////////////////////////////////////
}