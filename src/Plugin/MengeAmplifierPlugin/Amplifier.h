#	pragma once

#	include "Interface/AmplifierInterface.h"
#	include "Interface/SoundSystemInterface.h"
#	include "Interface/NotificationServiceInterface.h"

#   include "Core/ServiceBase.h"

#	include "Core/ConstString.h"

#	include "Config/Typedef.h"

#	include <stdex/stl_map.h>

namespace	Menge
{
	class Amplifier
		: public ServiceBase<AmplifierInterface>
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
		
	public:
		float getDuration() const override;
		
		void setPosMs( float _posMs ) override;
		float getPosMs() const override;

    protected:
        void onEngineFinalize();

    protected:
        ObserverInterfacePtr m_observerEngineFinalize;

		uint32_t m_sourceID;
		
		bool m_play;		
	};
}
