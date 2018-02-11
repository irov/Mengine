#	pragma once

#	include "Interface/ServiceInterface.h"

#	include "Utils/Core/ConstString.h"

namespace Menge
{
	class AmplifierInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE("Amplifier")

	public:		
		virtual bool playMusic( const ConstString & _resourceMusic, float _pos, bool _looped ) = 0;
		
		virtual void stop() = 0;
		virtual bool pause() = 0;
		virtual bool resume() = 0;

	public:
		virtual float getDuration() const = 0;
		
		virtual void setPosMs( float _posMs ) = 0;
		virtual float getPosMs() const = 0;
	};

#   define AMPLIFIER_SERVICE()\
    ((Menge::AmplifierInterface *)SERVICE_GET(Menge::AmplifierInterface))
}