#pragma once

#include "VideoInterface.h"

#include "Kernel/Surface.h"
#include "Kernel/BaseEventation.h"
#include "Kernel/BaseAnimation.h"
#include "Kernel/AnimationEventReceiver.h"

#include "ResourceVideo.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum ESurfaceVideoEventFlag
    {
        __EVENT_SURFACE_VIDEO_LAST__ = __EVENT_ANIMATION_LAST__
    };
    //////////////////////////////////////////////////////////////////////////
    class SurfaceVideoEventReceiverInterface
        : public AnimationEventReceiverInterface
    {
    };
    //////////////////////////////////////////////////////////////////////////
    class SurfaceVideo
        : public Surface
        , public UnknownVideoSurfaceInterface
        , protected BaseEventation
        , protected BaseAnimation
    {
        DECLARE_FACTORABLE( SurfaceVideo );
        DECLARE_UNKNOWABLE();
        DECLARE_ANIMATABLE();
        DECLARE_EVENTABLE();

    public:
        SurfaceVideo();
        ~SurfaceVideo() override;

    public:
        void setResourceVideo( const ResourcePtr & _resourceVideo ) override;
        const ResourcePtr & getResourceVideo() const override;

    public:
        float getWidth() const;
        float getHeight() const;

    protected:
        bool _compile() override;
        void _release() override;

    public:
        const mt::vec2f & getMaxSize() const override;
        const mt::vec2f & getSize() const override;
        const mt::vec2f & getOffset() const override;

        uint32_t getUVCount() const override;
        const mt::uv4f & getUV( uint32_t _index ) const override;

        void correctUV( uint32_t _index, const mt::vec2f & _in, mt::vec2f * const _out ) const override;

        const Color & getColor() const override;

    protected:
        void _activate() override;
        void _update( const UpdateContext * _context ) override;

    protected:
        void _setTime( float _time ) override;
        float _getTime() const override;
        float _getDuration() const override;

        void _setFirstFrame() override;
        void _setLastFrame() override;

    protected:
        bool _play( uint32_t _playId, float _time ) override;
        bool _restart( uint32_t _playId, float _time ) override;
        void _pause( uint32_t _playId ) override;
        void _resume( uint32_t _playId, float _time ) override;
        bool _stop( uint32_t _playId ) override;
        void _end( uint32_t _playId ) override;
        bool _interrupt( uint32_t _playId ) override;

    protected:
        RenderMaterialInterfacePtr _updateMaterial() const override;

    protected:
        void updateVideoBuffer_() const;

        bool sync_( float _time );
        void createDecoder_();
        bool fillVideoBuffer_() const;
        void updateUV_();

    protected:
        uint32_t getDecodeFlags_() const;

    protected:
        ResourceVideoPtr m_resourceVideo;

        RenderTextureInterfacePtr m_textures[1];

        VideoDecoderInterfacePtr m_decoderVideo;

        float m_time;

        mt::vec2f m_size;
        mt::uv4f m_uv;

        mutable bool m_needUpdateVideoBuffer;
        bool m_updateFirstFrame;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveSurfacePtr<SurfaceVideo> SurfaceVideoPtr;
    //////////////////////////////////////////////////////////////////////////
}
