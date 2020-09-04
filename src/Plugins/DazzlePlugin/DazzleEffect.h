#pragma once

#include "DazzleInterface.h"

#include "Kernel/Node.h"
#include "Kernel/BaseEventation.h"
#include "Kernel/BaseAnimation.h"
#include "Kernel/BaseUpdation.h"
#include "Kernel/BaseRender.h"

#include "dazzle/dazzle.hpp"

namespace Mengine
{
    class DazzleEffect
        : public Node
        , public BaseEventation
        , public BaseUpdation
        , public BaseRender
        , public BaseAnimation
        , public UnknownDazzleEffectInterface
    {
        DECLARE_VISITABLE( Node );
        DECLARE_UNKNOWABLE();
        DECLARE_ANIMATABLE();
        DECLARE_UPDATABLE();
        DECLARE_RENDERABLE();
        DECLARE_EVENTABLE();

    public:
        DazzleEffect();
        ~DazzleEffect() override;

    public:
        bool _play( uint32_t _enumerator, float _time ) override;
        bool _restart( uint32_t _enumerator, float _time ) override;
        void _pause( uint32_t _enumerator ) override;
        void _resume( uint32_t _enumerator, float _time ) override;
        bool _stop( uint32_t _enumerator ) override;
        void _end( uint32_t _enumerator ) override;
        bool _interrupt( uint32_t _enumerator ) override;

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

        void _updateBoundingBox( mt::box2f * _boundingBox, mt::box2f ** const _boundingBoxCurrent ) const override;

    protected:
        dz_effect_t * m_effect;
    };
}