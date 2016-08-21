#	pragma once

#	include "Core/ModuleBase.h"

#	include "Motor.h"

namespace Menge
{
	class ModuleMotor
		: public ModuleBase
	{
	public:
		ModuleMotor();
		~ModuleMotor();

	public:
		MotorPtr createMotor();
		
	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		void _update( float _time, float _timing ) override;
		void _render( const RenderObjectState * _state, unsigned int _debugMask ) override;

	protected:
		typedef stdex::vector<MotorPtr> TVectorMotors;
		TVectorMotors m_motors;
	};
}