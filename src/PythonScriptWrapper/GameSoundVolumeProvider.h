#   pragma once

#   include "Interface/SoundSystemInterface.h"

#	include "Core/ServantBase.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class Game;
    //////////////////////////////////////////////////////////////////////////
    class GameSoundVolumeProvider
        : public ServantBase<SoundVolumeProviderInterface>
    {
    public:
        GameSoundVolumeProvider();

	public:
		void setGame(Game * _game);
		Game * getGame() const;

    protected:
        void onSoundChangeVolume( float _sound, float _music, float _voice, bool _mute ) override;

    protected:
        Game * m_game;
    };
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<GameSoundVolumeProvider> GameSoundVolumeProviderPtr;
}