#pragma once

#include "Interface/ServantInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SoundVolumeProviderInterface
        : public ServantInterface
    {
    public:
        virtual void onSoundChangeVolume( float _sound, float _music, float _voice, bool _mute ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SoundVolumeProviderInterface> SoundVolumeProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}