#   pragma once

#   include "Interface/SoundSystemInterface.h"

#   include "Game.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    class GameSoundVolumeProvider
        : public SoundVolumeProviderInterface
    {
    public:
        GameSoundVolumeProvider( ServiceProviderInterface * _serviceProvider, Game * _game );

    protected:
        void onSoundChangeVolume( float _sound, float _music, float _voice ) override;

    protected:
        ServiceProviderInterface * m_serviceProvider;
        Game * m_game;
    };
}