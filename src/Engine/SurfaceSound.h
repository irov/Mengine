#pragma once

#include "Kernel/Surface.h"
#include "Kernel/Soundable.h"

#include "Kernel/BaseEventation.h"
#include "Kernel/AnimationEventReceiver.h"
#include "Kernel/BaseAnimation.h"

#include "Kernel/ResourceHolder.h"

#include "Engine/ResourceSound.h"

#include "Interface/SoundBufferInterface.h"
#include "Interface/SoundIdentityInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum SurfaceSoundEventFlag
    {
    };
    //////////////////////////////////////////////////////////////////////////
    class SurfaceSoundEventReceiver
        : public AnimationEventReceiver
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
        DECLARE_EVENTABLE( SurfaceSoundEventReceiver );

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

        void correctUV( uint32_t _index, mt::vec2f & _out, const mt::vec2f & _in ) override;

        const ColourValue & getColor() const override;

    protected:
        bool _play( uint32_t _enumerator, float _time ) override;
        bool _restart( uint32_t _enumerator, float _time ) override;
        void _pause( uint32_t _enumerator ) override;
        void _resume( uint32_t _enumerator, float _time ) override;
        void _stop( uint32_t _id ) override;
        void _end( uint32_t _id ) override;
        bool _interrupt( uint32_t _id ) override;
        void _setTime( float _time ) override;
        float _getTime() const override;

    protected:
        void _setVolume( float _volume ) override;
        float _getVolume() const override;

    public:
        float getDuration() const;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        bool _update( const UpdateContext * _context ) override;

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
        ResourceHolder<ResourceSound> m_resourceSound;

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
