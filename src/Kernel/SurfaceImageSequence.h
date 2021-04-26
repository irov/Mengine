#pragma once

#include "Kernel/Surface.h"
#include "Kernel/Resource.h"
#include "Kernel/BaseEventation.h"
#include "Kernel/BaseAnimation.h"
#include "Kernel/AnimationEventReceiver.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<class ResourceImageSequence> ResourceImageSequencePtr;
    //////////////////////////////////////////////////////////////////////////
    enum ESurfaceImageSequenceEventFlag
    {
        EVENT_SURFACE_IMAGESEQUENCE_FRAME_END = __EVENT_ANIMATION_LAST__,
        EVENT_SURFACE_IMAGESEQUENCE_FRAME_TICK,

        __EVENT_SURFACE_IMAGESEQUENCE_LAST__
    };
    //////////////////////////////////////////////////////////////////////////
    class SurfaceImageSequenceEventReceiverInterface
        : public AnimationEventReceiverInterface
    {
    public:
        virtual void onSurfaceImageSequenceFrameEnd( uint32_t _currentFrame ) = 0;
        virtual void onSurfaceImageSequenceFrameTick( uint32_t _currentFrame, uint32_t _frameCount ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    EVENTATION_TYPEID( SurfaceImageSequenceEventReceiverInterface, EVENT_SURFACE_IMAGESEQUENCE_FRAME_END );
    EVENTATION_TYPEID( SurfaceImageSequenceEventReceiverInterface, EVENT_SURFACE_IMAGESEQUENCE_FRAME_TICK );
    //////////////////////////////////////////////////////////////////////////
    class SurfaceImageSequence
        : public Surface
        , protected BaseEventation
        , protected BaseAnimation
    {
        DECLARE_FACTORABLE( SurfaceImageSequence );
        DECLARE_ANIMATABLE();
        DECLARE_EVENTABLE();

    public:
        SurfaceImageSequence();
        ~SurfaceImageSequence() override;

    public:
        void setResourceImageSequence( const ResourceImageSequencePtr & _resourceImageSequence );
        const ResourceImageSequencePtr & getResourceImageSequence() const;

    public:
        uint32_t getFrameCount() const;

        float getFrameDelay( uint32_t  _frame ) const;

        void setCurrentFrame( uint32_t _frame );
        uint32_t getCurrentFrame() const;

    public:
        const mt::vec2f & getMaxSize() const override;
        const mt::vec2f & getSize() const override;
        const mt::vec2f & getOffset() const override;

        uint32_t getUVCount() const override;
        const mt::uv4f & getUV( uint32_t _index ) const override;

        void correctUV( uint32_t _index, const mt::vec2f & _in, mt::vec2f * const _out ) const override;

        const Color & getColor() const override;

    protected:
        void _setTime( float _timming ) override;
        float _getTime() const override;

        void _setFirstFrame() override;
        void _setLastFrame() override;

    protected:
        bool _play( uint32_t _enumerator, float _time ) override;
        bool _restart( uint32_t _enumerator, float _time ) override;
        void _pause( uint32_t _enumerator ) override;
        void _resume( uint32_t _enumerator, float _time ) override;
        bool _stop( uint32_t _enumerator ) override;
        void _end( uint32_t _enumerator ) override;
        bool _interrupt( uint32_t _enumerator ) override;

    protected:
        void _update( const UpdateContext * _context ) override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        uint32_t getFrame_( float _time, float * const _deltaTime ) const;

    protected:
        RenderMaterialInterfacePtr _updateMaterial() const override;

    protected:
        ResourceImageSequencePtr m_resourceImageSequence;

        float m_frameTime;

        uint32_t m_currentFrame;

        Vector<RenderMaterialInterfacePtr> m_materials;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveSurfacePtr<SurfaceImageSequence> SurfaceImageSequencePtr;
    //////////////////////////////////////////////////////////////////////////
}
