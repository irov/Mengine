#include "SurfaceSound.h"

#include "Interface/RenderSystemInterface.h"
#include "Interface/SoundServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"

#include "ResourceSound.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"

#include <math.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SurfaceSound::SurfaceSound()
        : m_sourceCategory( ES_SOURCE_CATEGORY_SOUND )
        , m_interpolateVolume( true )
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
            LOGGER_ERROR( "sound '%s' resource  is null"
                , this->getName().c_str()
            );

            return false;
        }

        if( m_resourceSound.compile() == false )
        {
            LOGGER_ERROR( "sound '%s' resource '%s' not compile"
                , this->getName().c_str()
                , m_resourceSound->getName().c_str()
            );

            return false;
        }

        m_soundBuffer = m_resourceSound->createSoundBuffer();

        if( m_soundBuffer == nullptr )
        {
            LOGGER_ERROR( "sound '%s' resource '%s' sound buffer not create"
                , this->getName().c_str()
                , m_resourceSound->getName().c_str()
            );

            return false;
        }

        bool streamable = m_resourceSound->isStreamable();

        m_soundEmitter = SOUND_SERVICE()
            ->createSoundIdentity( m_isHeadMode, m_soundBuffer, m_sourceCategory, streamable
                , MENGINE_DOCUMENT( "sound '%s' resource '%s'", this->getName().c_str(), m_resourceSound->getName().c_str() ) );

        if( m_soundEmitter == nullptr )
        {
            LOGGER_ERROR( "sound '%s' resource '%s' not compiled [category '%d' streamable '%d']"
                , this->getName().c_str()
                , m_resourceSound->getName().c_str()
                , m_sourceCategory
                , streamable
            );

            return false;
        }

        m_soundEmitter->setSoundListener( SoundListenerInterfacePtr( this ) );

        AnimationInterface * animation = this->getAnimation();
        bool loop = animation->isLoop();

        SOUND_SERVICE()
            ->setLoop( m_soundEmitter, loop );

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
            LOGGER_ERROR( "'%s' emitter invalid release sound %d"
                , this->getName().c_str()
                , m_soundEmitter->getId()
            );
        }

        m_soundEmitter = nullptr;
        m_soundBuffer = nullptr;

        m_resourceSound.release();
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceSound::_update( const UpdateContext * _context )
    {
        (void)_context;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceSound::setResourceSound( const ResourceSoundPtr & _resourceSound )
    {
        if( m_resourceSound == _resourceSound )
        {
            return;
        }

        this->recompile( [this, _resourceSound]() {m_resourceSound = _resourceSound; } );
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceSoundPtr & SurfaceSound::getResourceSound() const
    {
        return m_resourceSound;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceSound::setSoundCategory( ESoundSourceCategory _sourceCategory )
    {
        m_sourceCategory = _sourceCategory;
    }
    //////////////////////////////////////////////////////////////////////////
    ESoundSourceCategory SurfaceSound::getSoundCategory() const
    {
        return m_sourceCategory;
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
    const Color & SurfaceSound::getColor() const
    {
        return Color::identity();
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceSound::_play( uint32_t _enumerator, float _time )
    {
        (void)_enumerator;
        (void)_time;

        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "'%s' not compile"
                , this->getName().c_str()
            );

            return false;
        }

        if( SOUND_SERVICE()
            ->playEmitter( m_soundEmitter ) == false )
        {
            LOGGER_ERROR( "'%s' invalid play [%d] resource '%s'"
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

        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "'%s' not compile"
                , this->getName().c_str()
            );

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

        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "'%s' not compile"
                , this->getName().c_str()
            );

            return;
        }

        SOUND_SERVICE()
            ->resumeEmitter( m_soundEmitter );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceSound::_stop( uint32_t _enumerator )
    {
        if( m_soundEmitter != nullptr )
        {
            if( SOUND_SERVICE()
                ->isEmitterStop( m_soundEmitter ) == false )
            {
                SOUND_SERVICE()
                    ->stopEmitter( m_soundEmitter );
            }
        }

        EVENTABLE_METHOD( EVENT_ANIMATION_END )
            ->onAnimationStop( _enumerator );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceSound::_end( uint32_t _enumerator )
    {
        //if( m_soundEmitter != nullptr )
        //{
        //    SOUND_SERVICE()
        //        ->stopEmitter( m_soundEmitter );
        //}

        EVENTABLE_METHOD( EVENT_ANIMATION_END )
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

        if( this->isCompile() == false )
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
            LOGGER_ERROR( "invalid %s:%d %f"
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
        if( this->isCompile() == false )
        {
            return;
        }

        SOUND_SERVICE()
            ->setLoop( m_soundEmitter, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    float SurfaceSound::_getDuration() const
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "'%s' not compile"
                , this->getName().c_str()
            );

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
    void SurfaceSound::_setTime( float _time )
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "'%s' not compile"
                , this->getName().c_str()
            );

            return;
        }

        float lengthMs = SOUND_SERVICE()
            ->getDuration( m_soundEmitter );

        float pos = _time;
        if( _time > lengthMs )
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
            LOGGER_ERROR( "'%s' not compile"
                , this->getName().c_str()
            );

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
    //////////////////////////////////////////////////////////////////////////
    void SurfaceSound::onSoundPause( const SoundIdentityInterfacePtr & _emitter )
    {
        MENGINE_UNUSED( _emitter );

        uint32_t id = this->getPlayId();

        EVENTABLE_METHOD( EVENT_ANIMATION_PAUSE )
            ->onAnimationPause( id );
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceSound::onSoundResume( const SoundIdentityInterfacePtr & _emitter )
    {
        MENGINE_UNUSED( _emitter );

        uint32_t id = this->getPlayId();

        EVENTABLE_METHOD( EVENT_ANIMATION_RESUME )
            ->onAnimationResume( id, 0.f );
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceSound::onSoundStop( const SoundIdentityInterfacePtr & _emitter )
    {
        MENGINE_UNUSED( _emitter );

        this->stop();
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceSound::onSoundEnd( const SoundIdentityInterfacePtr & _emitter )
    {
        MENGINE_UNUSED( _emitter );

        this->end();
    }
}
