#pragma once

#include "Kernel/Surface.h"
#include "Kernel/Soundable.h"

#include "Kernel/BaseEventation.h"
#include "Kernel/AnimationEventReceiver.h"
#include "Kernel/BaseAnimation.h"

#include "Engine/ResourceSound.h"

#include "Interface/SoundBufferInterface.h"
#include "Interface/SoundIdentityInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum ESurfaceSoundEventFlag
    {
        __EVENT_SURFACE_SOUND_LAST__ = __EVENT_ANIMATION_LAST__
    };
    //////////////////////////////////////////////////////////////////////////
    class SurfaceSoundEventReceiverInterface
        : public AnimationEventReceiverInterface
    {
    public:
    };
    //////////////////////////////////////////////////////////////////////////
    class SurfaceSound
        : public Surface
        , public Soundable
        , public BaseEventation
        , public BaseAnimation
        , public SoundListenerInterface
    {
        DECLARE_ANIMATABLE();
        DECLARE_EVENTABLE();

    public:
        SurfaceSound();
        ~SurfaceSound() override;

    public:
        void setResourceSound( const ResourceSoundPtr & _resourceSound );
        const ResourceSoundPtr & getResourceSound() const;

    public:
        void setSoundCategory( ESoundSourceCategory _sourceCategory );
        ESoundSourceCategory getSoundCategory() const;

    public:
        void setInterpolateVolume( bool _interpolateVolume );
        bool getInterpolateVolume() const;

    protected:
        const mt::vec2f & getMaxSize() const override;
        const mt::vec2f & getSize() const override;
        const mt::vec2f & getOffset() const override;

        uint32_t getUVCount() const override;
        const mt::uv4f & getUV( uint32_t _index ) const override;

        void correctUV( uint32_t _index, const mt::vec2f & _in, mt::vec2f * _out ) const override;

        const Color & getColor() const override;

    protected:
        bool _play( uint32_t _enumerator, float _time ) override;
        bool _restart( uint32_t _enumerator, float _time ) override;
        void _pause( uint32_t _enumerator ) override;
        void _resume( uint32_t _enumerator, float _time ) override;
        bool _stop( uint32_t _id ) override;
        void _end( uint32_t _id ) override;
        bool _interrupt( uint32_t _id ) override;
        void _setTime( float _time ) override;
        float _getTime() const override;

    protected:
        void _setVolume( float _volume ) override;
        float _getVolume() const override;

    public:
        float _getDuration() const override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        bool update( const UpdateContext * _context ) override;

    protected:
        void _setLoop( bool _value ) override;

    protected:
        void onSoundPause( const SoundIdentityInterfacePtr & _emitter ) override;
        void onSoundResume( const SoundIdentityInterfacePtr & _emitter ) override;
        void onSoundStop( const SoundIdentityInterfacePtr & _emitter ) override;
        void onSoundEnd( const SoundIdentityInterfacePtr & _emitter ) override;

    protected:
        RenderMaterialInterfacePtr _updateMaterial() const override;

    private:
        ResourceSoundPtr m_resourceSound;

        SoundBufferInterfacePtr m_soundBuffer;
        SoundIdentityInterfacePtr m_soundEmitter;

        ESoundSourceCategory m_sourceCategory;

        bool m_interpolateVolume;
        bool m_isHeadMode;

        float m_volume;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SurfaceSound> SurfaceSoundPtr;
};
