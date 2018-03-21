#pragma once

#include "Interface/AmplifierInterface.h"
#include "Interface/SoundSystemInterface.h"

#include "Core/ServiceBase.h"

#include "Core/ConstString.h"

#include "Config/Typedef.h"

#include "stdex/stl_map.h"

namespace Mengine
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

    protected:
        void _stop() override;

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
		uint32_t m_sourceID;
		
		bool m_play;		
	};
}
