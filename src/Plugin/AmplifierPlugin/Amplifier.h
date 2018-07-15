#pragma once

#include "Interface/AmplifierInterface.h"
#include "Interface/SoundSystemInterface.h"

#include "Kernel/ServiceBase.h"

#include "Kernel/ConstString.h"

#include "Config/Typedef.h"

#include "Config/Map.h"

namespace Mengine
{
	class Amplifier
		: public ServiceBase<AmplifierInterface>
	{
	public:
		Amplifier();
		~Amplifier() override;

	public:
		bool _initializeService() override;
		void _finalizeService() override;

    protected:
        void _stopService() override;

	public:
        bool playMusic( const ConstString & _resourceMusic, float _pos, bool _looped ) override;
				
		void stopMusic() override;
		bool pauseMusic() override;
		bool resumeMusic() override;
		
	public:
		float getDuration() const override;
		
		void setPosMs( float _posMs ) override;
		float getPosMs() const override;

    protected:
        SoundIdentityInterfacePtr m_soundEmitter;
		
		bool m_play;		
	};
}
