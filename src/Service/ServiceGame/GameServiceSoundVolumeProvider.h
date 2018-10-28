#pragma once

#include "Interface/SoundVolumeProviderInterface.h"

#include "Kernel/Eventable.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class GameServiceSoundVolumeProvider
        : public SoundVolumeProviderInterface
        , public Factorable
    {
    public:
		GameServiceSoundVolumeProvider();
		~GameServiceSoundVolumeProvider() override;

	public:
		void setEventable(const EventablePtr & _eventable);
		const EventablePtr & getEventable() const;

    protected:
        void onSoundChangeVolume( float _sound, float _music, float _voice, bool _mute ) override;

    protected:
		EventablePtr m_eventable;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<GameServiceSoundVolumeProvider> GameSoundVolumeProviderPtr;
	//////////////////////////////////////////////////////////////////////////
}