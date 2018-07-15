#include "SurfaceSound.h"

#include "Interface/RenderSystemInterface.h"
#include "Interface/ResourceInterface.h"

#include "ResourceSound.h"

#include "Kernel/Logger.h"

#include <math.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SurfaceSound::MySoundListener
        : public FactorableUnique<SoundListenerInterface>
    {
    public:
        MySoundListener( uint32_t _id, SurfaceSound * _sound )
            : m_id( _id )
            , m_sound( _sound )
        {
        }

        ~MySoundListener() override
        {
        }

    protected:
        void onSoundPause( const SoundIdentityInterfacePtr & _emitter ) override
        {
            (void)_emitter;

            EVENTABLE_METHOD( m_sound, EVENT_ANIMATION_PAUSE )
                ->onAnimationPause( m_id );
        }

        void onSoundStop( const SoundIdentityInterfacePtr & _emitter ) override
        {
            (void)_emitter;

            m_sound->end();
        }

    protected:
        uint32_t m_id;
        SurfaceSound * m_sound;
    };
    //////////////////////////////////////////////////////////////////////////
    SurfaceSound::SurfaceSound()
        : m_interpolateVolume( true )
        , m_isHeadMode( false )
        , m_volume( 1.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SurfaceSound::~SurfaceSound()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceSound::_compile()
    {
        if( m_resourceSound == nullptr )
        {
            LOGGER_ERROR( "SoundEmitter::_compile: '%s' resource is null"
                , this->getName().c_str()
            );

            return false;
        }

        if( m_resourceSound.compile() == false )
        {
            LOGGER_ERROR( "SoundEmitter::_compile: '%s' resource '%s' not compile"
                , this->getName().c_str()
                , m_resourceSound->getName().c_str()
            );

            return false;
        }

        m_soundBuffer = m_resourceSound->createSoundBuffer();

        if( m_soundBuffer == nullptr )
        {
            LOGGER_ERROR( "SoundEmitter::_compile: '%s' sound buffer not create"
                , this->getName().c_str()
            );

            return false;
        }

        bool streamable = m_resourceSound->isStreamable();

        m_soundEmitter = SOUND_SERVICE()
            ->createSoundIdentity( m_isHeadMode, m_soundBuffer, ESST_SOUND, streamable );

        if( m_soundEmitter == nullptr )
        {
            LOGGER_ERROR( "SoundEmitter::_compile: sound emitter '%s' not compiled"
                , this->getName().c_str()
            );

            return false;
        }

        SOUND_SERVICE()
            ->setSourceListener( m_soundEmitter, new MySoundListener( m_enumerator, this ) );

        SOUND_SERVICE()
            ->setLoop( m_soundEmitter, m_loop );

        float volume = m_resourceSound->getDefaultVolume();
        this->setVolume( volume );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceSound::_release()
    {
        if( SOUND_SERVICE()
            ->releaseSoundSource( m_soundEmitter ) == false )
        {
            LOGGER_ERROR( "SoundEmitter::_release %s emitter invalid release sound %d"
                , this->getName().c_str()
                , m_soundEmitter->getId()
            );
        }

        m_soundEmitter = nullptr;
        m_soundBuffer = nullptr;

        m_resourceSound.release();
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceSound::_update( float _time, float _timing )
    {
        (void)_time;
        (void)_timing;
        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceSound::setResourceSound( const ResourceSoundPtr & _resourceSound )
    {
        if( m_resourceSound == _resourceSound )
        {
            return;
        }

        m_resourceSound = _resourceSound;

        this->recompile();
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceSoundPtr & SurfaceSound::getResourceSound() const
    {
        return m_resourceSound;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceSound::setInterpolateVolume( bool _interpolateVolume )
    {
        m_interpolateVolume = _interpolateVolume;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceSound::getInterpolateVolume() const
    {
        return m_interpolateVolume;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & SurfaceSound::getMaxSize() const
    {
        return mt::vec2f::identity();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & SurfaceSound::getSize() const
    {
        return mt::vec2f::identity();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & SurfaceSound::getOffset() const
    {
        return mt::vec2f::identity();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t SurfaceSound::getUVCount() const
    {
        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::uv4f & SurfaceSound::getUV( uint32_t _index ) const
    {
        (void)_index;

        return mt::uv4f::identity();
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceSound::correctUV( uint32_t _index, mt::vec2f & _out, const mt::vec2f & _in )
    {
        (void)_index;

        _out = _in;
    }
    //////////////////////////////////////////////////////////////////////////
    const ColourValue & SurfaceSound::getColor() const
    {
        return ColourValue::identity();
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceSound::_play( uint32_t _enumerator, float _time )
    {
        (void)_enumerator;
        (void)_time;

        if( this->isCompile() == false )
        {
            return false;
        }

        if( m_soundEmitter == nullptr )
        {
            return false;
        }

        if( SOUND_SERVICE()
            ->playEmitter( m_soundEmitter ) == false )
        {
            LOGGER_ERROR( "SoundEmitter::_play %s invalid play [%d] resource %s"
                , this->getName().c_str()
                , m_soundEmitter->getId()
                , m_resourceSound->getName().c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceSound::_restart( uint32_t _enumerator, float _time )
    {
        (void)_time;
        (void)_enumerator;
        //ToDo

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceSound::_pause( uint32_t _enumerator )
    {
        (void)_enumerator;

        if( m_soundEmitter == nullptr )
        {
            return;
        }

        SOUND_SERVICE()
            ->pauseEmitter( m_soundEmitter );
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceSound::_resume( uint32_t _enumerator, float _time )
    {
        (void)_time;
        (void)_enumerator;

        if( m_soundEmitter == nullptr )
        {
            return;
        }

        SOUND_SERVICE()
            ->resumeEmitter( m_soundEmitter );
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceSound::_stop( uint32_t _enumerator )
    {
        if( m_soundEmitter != nullptr )
        {
            SOUND_SERVICE()
                ->stopEmitter( m_soundEmitter );
        }

        EVENTABLE_METHOD( this, EVENT_ANIMATION_END )
            ->onAnimationStop( _enumerator );
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceSound::_end( uint32_t _enumerator )
    {
        if( m_soundEmitter != nullptr )
        {
            SOUND_SERVICE()
                ->stopEmitter( m_soundEmitter );
        }

        EVENTABLE_METHOD( this, EVENT_ANIMATION_END )
            ->onAnimationEnd( _enumerator );
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceSound::_setVolume( float _volume )
    {
        if( fabsf( m_volume - _volume ) < 0.00001f )
        {
            return;
        }

        m_volume = _volume;

        if( m_soundEmitter == nullptr )
        {
            return;
        }

        bool forceVolume = false;

        if( m_interpolateVolume == false )
        {
            forceVolume = true;
        }

        if( this->isPlay() == false )
        {
            forceVolume = true;
        }

        if( SOUND_SERVICE()
            ->setSourceVolume( m_soundEmitter, m_volume, m_volume, forceVolume ) == false )
        {
            LOGGER_ERROR( "SoundEmitter::setVolume invalid %s:%d %f"
                , m_resourceSound->getName().c_str()
                , m_soundEmitter->getId()
                , m_volume
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    float SurfaceSound::_getVolume() const
    {
        return m_volume;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceSound::_setLoop( bool _value )
    {
        (void)_value;

        if( m_soundEmitter == nullptr )
        {
            return;
        }

        SOUND_SERVICE()
            ->setLoop( m_soundEmitter, m_loop );
    }
    //////////////////////////////////////////////////////////////////////////
    float SurfaceSound::getDuration() const
    {
        if( m_soundEmitter == nullptr )
        {
            return 0.f;
        }

        float lengthMs = SOUND_SERVICE()
            ->getDuration( m_soundEmitter );

        return lengthMs;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceSound::_interrupt( uint32_t _enumerator )
    {
        (void)_enumerator;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceSound::_setTime( float _timing )
    {
        if( this->isCompile() == false )
        {
            return;
        }

        if( m_soundEmitter == nullptr )
        {
            return;
        }

        float lengthMs = SOUND_SERVICE()
            ->getDuration( m_soundEmitter );

        float pos = _timing;
        if( _timing > lengthMs )
        {
            pos = lengthMs;
        }

        SOUND_SERVICE()
            ->setPosMs( m_soundEmitter, pos );
    }
    //////////////////////////////////////////////////////////////////////////
    float SurfaceSound::_getTime() const
    {
        if( this->isCompile() == false )
        {
            return 0.f;
        }

        if( m_soundEmitter == nullptr )
        {
            return 0.f;
        }
        
        float pos = SOUND_SERVICE()
            ->getPosMs( m_soundEmitter );

        return pos;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderMaterialInterfacePtr SurfaceSound::_updateMaterial() const
    {
        return nullptr;
    }
}
