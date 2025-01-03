#include "AmplifierSoundListener.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AmplifierSoundListener::AmplifierSoundListener( const AmplifierMusicCallbackInterfacePtr & _callback )
        : m_callback( _callback )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AmplifierSoundListener::~AmplifierSoundListener()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AmplifierSoundListener::onSoundPause( const SoundIdentityInterfacePtr & _identity )
    {
        m_callback->onMusicPause( _identity );
    }
    //////////////////////////////////////////////////////////////////////////
    void AmplifierSoundListener::onSoundResume( const SoundIdentityInterfacePtr & _identity )
    {
        m_callback->onMusicResume( _identity );
    }
    //////////////////////////////////////////////////////////////////////////
    void AmplifierSoundListener::onSoundStop( const SoundIdentityInterfacePtr & _identity )
    {
        m_callback->onMusicStop( _identity );
    }
    //////////////////////////////////////////////////////////////////////////
    void AmplifierSoundListener::onSoundEnd( const SoundIdentityInterfacePtr & _identity )
    {
        m_callback->onMusicEnd( _identity );
    }
}//////////////////////////////////////////////////////////////////////////
