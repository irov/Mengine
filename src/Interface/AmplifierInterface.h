#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/ConstString.h"

namespace Mengine
{
	class AmplifierInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE("Amplifier")

	public:		
		virtual bool playMusic( const ConstString & _resourceMusic, float _pos, bool _looped ) = 0;
		
		virtual void stopMusic() = 0;
		virtual bool pauseMusic() = 0;
		virtual bool resumeMusic() = 0;

	public:
		virtual float getDuration() const = 0;
		
		virtual void setPosMs( float _posMs ) = 0;
		virtual float getPosMs() const = 0;
	};

#   define AMPLIFIER_SERVICE()\
    ((Mengine::AmplifierInterface *)SERVICE_GET(Mengine::AmplifierInterface))
}