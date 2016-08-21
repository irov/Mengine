#	include "MotorPlugin.h"

#	include "Motor.h"

#	include "Kernel/Node.h"
#	include "Logger/Logger.h"

#	include "pybind/pybind.hpp"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY(Motor, Menge::MotorPlugin)
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MotorPlugin::MotorPlugin()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool MotorPlugin::_initialize()
	{		
		//if( PROTOTYPE_SERVICE( m_serviceProvider )
		//	->addPrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Motor" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "Motor" ), new MotorPrototypeGenerator<Motor, 32> ) == false )
		//{
		//	return false;
		//}

		//MODULE_SERVICE(m_serviceProvider)
		//	->registerModule(STRINGIZE_STRING_LOCAL(m_serviceProvider, "ModuleMotor")
		//	, new ModuleFactory<ModuleMotor>(m_serviceProvider, STRINGIZE_STRING_LOCAL(m_serviceProvider, "ModuleMotor")));

		pybind::interface_<Motor, pybind::bases<Affector> >( "Motor" )
			.def( "addVelocity", &Motor::addVelocity )
			.def( "hasVelocity", &Motor::hasVelocity )
			.def( "getVelocity", &Motor::getVelocity )
			;

		pybind::def_functor( "createMotor", this, &MotorPlugin::createMotor );
		pybind::def_functor( "destroyMotor", this, &MotorPlugin::destroyMotor );
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MotorPlugin::_finalize()
	{
		//MODULE_SERVICE(m_serviceProvider)
		//	->unregisterModule(STRINGIZE_STRING_LOCAL(m_serviceProvider, "ModuleMotor"));
	}
	//////////////////////////////////////////////////////////////////////////
	Motor * MotorPlugin::createMotor( Node * _node )
	{
		Motor * motor = m_factoryMotors.createObject();

		motor->setServiceProvider( m_serviceProvider );
		motor->setNode( _node );

		_node->addAffector( motor );

		return motor;
	}
	//////////////////////////////////////////////////////////////////////////
	void MotorPlugin::destroyMotor( Motor * _motor )
	{
		Node * node = _motor->getNode();

		uint32_t id = _motor->getId();

		node->stopAffector( id );
	}

}
