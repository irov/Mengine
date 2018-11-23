#include "GameServiceSoundVolumeProvider.h"

#include "Interface/GameServiceInterface.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    GameServiceSoundVolumeProvider::GameServiceSoundVolumeProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    GameServiceSoundVolumeProvider::~GameServiceSoundVolumeProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void GameServiceSoundVolumeProvider::setEventable( const EventablePtr & _eventable )
    {
        m_eventable = _eventable;
    }
    //////////////////////////////////////////////////////////////////////////
    const EventablePtr & GameServiceSoundVolumeProvider::getEventable() const
    {
        return m_eventable;
    }
    //////////////////////////////////////////////////////////////////////////
    void GameServiceSoundVolumeProvider::onSoundChangeVolume( float _sound, float _music, float _voice, bool _mute )
    {
        (void)_mute;

        EVENTABLE_METHODT( m_eventable, EVENT_GAME_CHANGE_SOUND_VOLUME, GameEventReceiver )
            ->onGameChangeSoundVolume( _sound, _music, _voice );
    }
}