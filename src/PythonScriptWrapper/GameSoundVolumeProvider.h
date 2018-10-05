#pragma once

#include "Interface/SoundVolumeProviderInterface.h"

#include "Kernel/ServantBase.h"

namespace Mengine
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
        void setGame( Game * _game );
        Game * getGame() const;

    protected:
        void onSoundChangeVolume( float _sound, float _music, float _voice, bool _mute ) override;

    protected:
        Game * m_game;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<GameSoundVolumeProvider> GameSoundVolumeProviderPtr;
}