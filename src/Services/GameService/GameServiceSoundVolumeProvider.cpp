#include "GameServiceSoundVolumeProvider.h"

#include "Interface/GameServiceInterface.h"

#include "Kernel/EventableHelper.h"

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
        MENGINE_UNUSED( _mute );

        EVENTABLE_OTHER_METHODT( m_eventable, EVENT_GAME_CHANGE_SOUND_VOLUME, GameEventReceiverInterface )
            ->onGameChangeSoundVolume( _sound, _music, _voice );
    }
}