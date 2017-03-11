#   include "GameSoundVolumeProvider.h"

#	include "Game.h"

//////////////////////////////////////////////////////////////////////////
namespace Menge
{//////////////////////////////////////////////////////////////////////////
    GameSoundVolumeProvider::GameSoundVolumeProvider()
		: m_game(nullptr)
    {
    }
	//////////////////////////////////////////////////////////////////////////
	void GameSoundVolumeProvider::setGame(Game * _game)
	{
		m_game = _game;
	}
	//////////////////////////////////////////////////////////////////////////
	Game * GameSoundVolumeProvider::getGame() const
	{
		return m_game;
	}
    //////////////////////////////////////////////////////////////////////////
    void GameSoundVolumeProvider::onSoundChangeVolume( float _sound, float _music, float _voice, bool _mute )
    {
        (void)_mute;

        EVENTABLE_METHOD( m_game, EVENT_GAME_CHANGE_SOUND_VOLUME )
            ->onGameChangeSoundVolume( _sound, _music, _voice );
        //EVENTABLE_CALL(m_serviceProvider, m_game, EVENT_GAME_CHANGE_SOUND_VOLUME)( _sound, _music, _voice );
    }
}