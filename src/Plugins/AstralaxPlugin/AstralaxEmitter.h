#pragma once

#include "AstralaxInterface.h"

#include "ResourceAstralax.h"

#include "Kernel/Node.h"
#include "Kernel/BaseEventation.h"
#include "Kernel/BaseAnimation.h"
#include "Kernel/BaseUpdation.h"
#include "Kernel/BaseRender.h"
#include "Kernel/BaseTransformation.h"
#include "Kernel/AnimationEventReceiver.h"
#include "Kernel/Polygon.h"
#include "Kernel/Vector.h"

#include "math/mat3.h"

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_PLUGIN_ASTRALAX_MAX_ATLAS_TEXTURE
#define MENGINE_PLUGIN_ASTRALAX_MAX_ATLAS_TEXTURE 8
#endif
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EAstralaxEmitter2EventFlag
    {
        __EVENT_ASTRALAX_EMITTER2_LAST__ = __EVENT_ANIMATION_LAST__
    };
    //////////////////////////////////////////////////////////////////////////
    class AstralaxEmitter2EventReceiverInterface
        : public AnimationEventReceiverInterface
    {
    public:
    };
    //////////////////////////////////////////////////////////////////////////
    class AstralaxEmitter
        : public Node
        , public AstralaxPositionProviderInterface
        , public AstralaxCameraProviderInterface
        , public UnknownAstralaxEmitterInterface
        , protected BaseEventation
        , protected BaseUpdation
        , protected BaseRender
        , protected BaseAnimation
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( AstralaxEmitter );
        DECLARE_VISITABLE( Node );
        DECLARE_UNKNOWABLE();
        DECLARE_ANIMATABLE();
        DECLARE_UPDATABLE();
        DECLARE_RENDERABLE();
        DECLARE_EVENTABLE();
        DECLARE_TRANSFORMABLE();

    public:
        AstralaxEmitter();
        ~AstralaxEmitter() override;

    public:
        bool _play( uint32_t _enumerator, float _time ) override;
        bool _restart( uint32_t _enumerator, float _time ) override;
        void _pause( uint32_t _enumerator ) override;
        void _resume( uint32_t _enumerator, float _time ) override;
        bool _stop( uint32_t _enumerator ) override;
        void _end( uint32_t _enumerator ) override;
        bool _interrupt( uint32_t _enumerator ) override;

    public:
        void setEmitterTranslateWithParticle( bool _translateWithParticle ) override;
        void setEmitterPositionRelative( bool _positionRelative ) override;
        void setEmitterCameraRelative( bool _cameraRelative ) override;

        void setEmitterPositionProviderOriginOffset( const mt::vec3f & _originOffset ) override;

        void changeEmitterImage( const ResourceImagePtr & _emitterResourceImage );
        void removeEmitterImage();

        bool changeEmitterPolygon( const Polygon & _polygon );
        void removeEmitterPolygon();

        void setResourceAstralax( const ResourcePtr & _resource ) override;
        const ResourcePtr & getResourceAstralax() const override;
        
        void setEmitterRandomMode( bool _randomMode );
        bool getEmitterRandomMode() const;

    protected:
        float _getDuration() const override;

    protected:
        void _setLoop( bool _value ) override;

    protected:
        bool _activate() override;
        void _deactivate() override;

        bool _compile() override;
        void _release() override;

        void update( const UpdateContext * _context ) override;
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

        void _updateBoundingBox( mt::box2f * const _boundingBox, mt::box2f ** const _boundingBoxCurrent ) const override;

    private:
        bool compileEmitterImage_( const AstralaxEmitterInterfacePtr & _emitter );
        bool compilePolygon_( const AstralaxEmitterInterfacePtr & _emitter );

    protected:
        void updateVertexColor_( RenderVertex2D * const _vertices, uint32_t _verticesCount ) const;
        void updateVertexWM_( RenderVertex2D * const _vertices, uint32_t _verticesCount ) const;

    protected:
        void onProviderEmitterPosition( mt::vec3f * _position ) override;
        void onProviderEmitterCamera( bool * const _orthogonality, mt::vec3f * const _position, mt::vec3f * const _direction ) override;

    protected:
        ResourceAstralaxPtr m_resourceAstralax;

        ResourceImagePtr m_emitterResourceImage;

        AstralaxEmitterInterfacePtr m_emitter;

        Polygon m_polygon;

        mt::vec3f m_positionProviderOriginOffset;

        bool m_emitterPositionRelative;
        bool m_emitterCameraRelative;

        bool m_randomMode;

        mutable RenderVertex2D * m_renderVertices;
        mutable uint32_t m_renderVertexCount;

        mutable RenderIndex * m_renderIndicies;
        mutable uint32_t m_renderIndexCount;

        bool m_emitterTranslateWithParticle;
        bool m_emitterTranslateWithParticleSetup;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AstralaxEmitter> AstralaxEmitterPtr;
    //////////////////////////////////////////////////////////////////////////
}
