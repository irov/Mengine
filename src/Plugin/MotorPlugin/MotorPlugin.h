#	pragma once

#	include "Core/PluginBase.h"

#	include "Motor.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class MotorPlugin
		: public PluginBase
	{
		PLUGIN_DECLARE( "Motor" )

	public:
		MotorPlugin();

	protected:
		bool _initialize() override;
		void _finalize() override;

	public:
		Motor * createMotor( Node * _node );
		void destroyMotor( Motor * _motor );

	protected:
		typedef FactoryPoolStore<Motor, 16> TFactoryPoolMotor;
		TFactoryPoolMotor m_factoryMotors;
	};
}