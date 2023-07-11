#include "SurfaceSound.h"

#include "Interface/SoundServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"

#include "Kernel/ResourceSound.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/EventableHelper.h"
#include "Kernel/ResourceHelper.h"

#include "Config/StdMath.h"

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
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceSound, "sound '%s' resource  is nullptr"
            , this->getName().c_str()
        );

        if( m_resourceSound->compile() == false )
        {
            LOGGER_ERROR( "surface sound '%s' resource '%s' not compile"
                , this->getName().c_str()
                , m_resourceSound->getName().c_str()
            );

            return false;
        }

        SoundBufferInterfacePtr soundBuffer = m_resourceSound->createSoundBuffer( MENGINE_DOCUMENT_FACTORABLE );

        if( soundBuffer == nullptr )
        {
            m_resourceSound->release();

            LOGGER_ERROR( "surface sound '%s' resource '%s' failed create buffer"
                , this->getName().c_str()
                , m_resourceSound->getName().c_str()
            );

            return false;
        }

        bool streamable = m_resourceSound->isStreamable();

        SoundIdentityInterfacePtr soundIdentity = SOUND_SERVICE()
            ->createSoundIdentity( m_isHeadMode, soundBuffer, m_sourceCategory, streamable
                , MENGINE_DOCUMENT_MESSAGE( "sound '%s' resource '%s'", this->getName().c_str(), m_resourceSound->getName().c_str() ) );

        MENGINE_ASSERTION_MEMORY_PANIC( soundIdentity, "sound '%s' resource '%s' not compiled category [%u] streamable [%u]"
            , this->getName().c_str()
            , m_resourceSound->getName().c_str()
            , m_sourceCategory
            , streamable
        );

        soundIdentity->setSoundListener( SoundListenerInterfacePtr( this ) );

        AnimationInterface * animation = this->getAnimation();
        bool loop = animation->calcTotalLoop();

        SOUND_SERVICE()
            ->setLoop( soundIdentity, loop );

        m_soundIdentity = soundIdentity;

        float volume = m_resourceSound->getDefaultVolume();
        this->setVolume( volume );

        LOGGER_INFO( "sound", "compile '%s' file '%s' %s"
            , this->getName().c_str()
            , Helper::getResourceFilePath( this->getResourceSound() ).c_str()
            , m_resourceSound->isStreamable() == true ? "[stream]" : ""
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceSound::_release()
    {
        LOGGER_INFO( "sound", "release '%s' file '%s' %s"
            , this->getName().c_str()
            , Helper::getResourceFilePath( this->getResourceSound() ).c_str()
            , m_resourceSound->isStreamable() == true ? "[stream]" : ""
        );

        if( SOUND_SERVICE()
            ->releaseSoundSource( m_soundIdentity ) == false )
        {
            LOGGER_ERROR( "surface sound '%s' emitter invalid release sound %u"
                , this->getName().c_str()
                , m_soundIdentity->getId()
            );
        }

        m_soundIdentity->setSoundListener( nullptr );

        m_soundIdentity = nullptr;

        m_resourceSound->release();
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceSound::correctUV( uint32_t _index, const mt::vec2f & _in, mt::vec2f * const _out ) const
    {
        MENGINE_UNUSED( _index );

        *_out = _in;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceSound::setResourceSound( const ResourceSoundPtr & _resourceSound )
    {
        if( m_resourceSound == _resourceSound )
        {
            return;
        }

        this->recompile( [this, &_resourceSound]()
        {
            m_resourceSound = _resourceSound;

            if( m_resourceSound == nullptr )
            {
                return false;
            }

            return true;
        } );
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
        MENGINE_UNUSED( _index );

        return mt::uv4f::identity();
    }
    //////////////////////////////////////////////////////////////////////////
    const Color & SurfaceSound::getColor() const
    {
        return Color::identity();
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceSound::_play( UniqueId _enumerator, float _time )
    {
        MENGINE_UNUSED( _enumerator );
        MENGINE_UNUSED( _time );

        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "surface sound '%s' not compile"
                , this->getName().c_str()
            );

            return false;
        }

        LOGGER_INFO( "sound", "[surface] sound play '%s' resource '%s' file '%s'"
            , this->getName().c_str()
            , m_resourceSound->getName().c_str()
            , Helper::getResourceFilePath( m_resourceSound ).c_str()
        );

        if( SOUND_SERVICE()
            ->playEmitter( m_soundIdentity ) == false )
        {
            LOGGER_ERROR( "surface sound '%s' invalid play [%u] resource '%s'"
                , this->getName().c_str()
                , m_soundIdentity->getId()
                , m_resourceSound->getName().c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceSound::_restart( UniqueId _enumerator, float _time )
    {
        MENGINE_UNUSED( _time );
        MENGINE_UNUSED( _enumerator );

        //ToDo

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceSound::_pause( UniqueId _enumerator )
    {
        MENGINE_UNUSED( _enumerator );

        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "surface sound '%s' not compile"
                , this->getName().c_str()
            );

            return;
        }

        SOUND_SERVICE()
            ->pauseEmitter( m_soundIdentity );
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceSound::_resume( UniqueId _enumerator, float _time )
    {
        MENGINE_UNUSED( _time );
        MENGINE_UNUSED( _enumerator );

        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "surface sound '%s' not compile"
                , this->getName().c_str()
            );

            return;
        }

        SOUND_SERVICE()
            ->resumeEmitter( m_soundIdentity );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceSound::_stop( UniqueId _enumerator )
    {
        if( m_soundIdentity != nullptr )
        {
            if( SOUND_SERVICE()
                ->isEmitterStop( m_soundIdentity ) == false )
            {
                SOUND_SERVICE()
                    ->stopEmitter( m_soundIdentity );
            }
        }

        EVENTABLE_METHOD( EVENT_ANIMATION_END )
            ->onAnimationStop( _enumerator );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceSound::_end( UniqueId _enumerator )
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
        if( MENGINE_FABSF( m_volume - _volume ) < 0.00001f )
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
            ->setSourceVolume( m_soundIdentity, m_volume, m_volume, forceVolume ) == false )
        {
            LOGGER_ERROR( "surface sound '%s' resource '%s' identity [%u] invalid set source volume %f"
                , this->getName().c_str()
                , m_resourceSound->getName().c_str()
                , m_soundIdentity->getId()
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
            ->setLoop( m_soundIdentity, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    float SurfaceSound::_getDuration() const
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "surface sound '%s' not compile"
                , this->getName().c_str()
            );

            return 0.f;
        }

        float lengthMs = SOUND_SERVICE()
            ->getDuration( m_soundIdentity );

        return lengthMs;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceSound::_interrupt( UniqueId _enumerator )
    {
        MENGINE_UNUSED( _enumerator );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceSound::_setTime( float _time )
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "surface sound '%s' not compile"
                , this->getName().c_str()
            );

            return;
        }

        float lengthMs = SOUND_SERVICE()
            ->getDuration( m_soundIdentity );

        float pos = _time;
        if( _time > lengthMs )
        {
            pos = lengthMs;
        }

        SOUND_SERVICE()
            ->setPosMs( m_soundIdentity, pos );
    }
    //////////////////////////////////////////////////////////////////////////
    float SurfaceSound::_getTime() const
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "surface sound '%s' not compile"
                , this->getName().c_str()
            );

            return 0.f;
        }

        float pos = SOUND_SERVICE()
            ->getPosMs( m_soundIdentity );

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
    //////////////////////////////////////////////////////////////////////////
}
