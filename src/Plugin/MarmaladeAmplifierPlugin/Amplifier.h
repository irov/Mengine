#	 pragma once

#	include "Interface/AmplifierInterface.h"
#	include "Interface/SoundSystemInterface.h"
#	include "Interface/MemoryInterface.h"

#	include "Core/ConstString.h"

#	include "Config/Typedef.h"

#	include <map>

namespace	Menge
{
	class Amplifier
		: public ServiceBase<AmplifierInterface>
		, public SoundVolumeProviderInterface
	{
	public:
		Amplifier();
		~Amplifier();
		
	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		bool playMusic( const ConstString & _resourceMusic, float _pos, bool _looped ) override;
						
		void stop() override;
		bool pause() override;
		bool resume() override;

		float getDuration() const override;
		
		void setPosMs( float _posMs ) override;
		float getPosMs() const override;

	protected:
		void onSoundChangeVolume( float _sound, float _music, float _voice ) override;

	protected:
        ServiceProviderInterface * m_serviceProvider;

		float m_volume;
		
		bool m_play;
		bool m_loop;

		MemoryInterfacePtr m_audioMemory;

	public:		
		void onSoundStop();
	};
}
