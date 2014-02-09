#	include "ModuleCollisionGround.h"

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/PlayerInterface.h"

#	include "PathFinderWay.h"

#	include "pybind/pybind.hpp"

//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ModuleCollisionGround::ModuleCollisionGround()
		: m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ModuleCollisionGround::~ModuleCollisionGround()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleCollisionGround::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * ModuleCollisionGround::getServiceProvider() const
	{
		return m_serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ModuleCollisionGround::initialize()
	{
		pybind::interface_<CollisionGround>("CollisionGround")
			.def( "addObject", &CollisionGround::addObject )
			.def( "removeObject", &CollisionGround::removeObject )
			.def( "addRadar", &CollisionGround::addRadar )
			.def( "removeRadar", &CollisionGround::removeRadar )
			;

		pybind::interface_<CollisionObject>("CollisionObject")			
			.def( "setNode", &CollisionObject::setNode )
			.def( "getNode", &CollisionObject::getNode )
			.def( "setRadius", &CollisionObject::setRadius )
			.def( "getRadius", &CollisionObject::getRadius )
			.def( "setMass", &CollisionObject::setMass )
			.def( "getMass", &CollisionObject::getMass )
			.def( "setMotor", &CollisionObject::setMotor )
			.def( "getMotor", &CollisionObject::getMotor )
			;

		pybind::interface_<PythonCollisionObject, pybind::bases<CollisionObject> >("PythonCollisionObject")		
			.def( "setPythonUser", &PythonCollisionObject::setPythonUser )
			.def( "getPythonUser", &PythonCollisionObject::getPythonUser )
			;


		pybind::interface_<CollisionRadar>("CollisionRadar")			
			.def( "setNode", &CollisionRadar::setNode )
			.def( "getNode", &CollisionRadar::getNode )
			.def( "setRadius", &CollisionRadar::setRadius )
			.def( "getRadius", &CollisionRadar::getRadius )
			;

		pybind::interface_<PythonCollisionRadar, pybind::bases<CollisionRadar> >("PythonCollisionRadar")			
			.def( "setNode", &CollisionRadar::setNode )
			.def( "getNode", &CollisionRadar::getNode )
			.def( "setRadius", &CollisionRadar::setRadius )
			.def( "getRadius", &CollisionRadar::getRadius )
			;

		pybind::interface_<CollisionMotor>("CollisionMotor")			
			.def( "setLinearSpeed", &CollisionMotor::setLinearSpeed )
			.def( "getLinearSpeed", &CollisionMotor::getLinearSpeed )
			.def( "setAngularSpeed", &CollisionMotor::setAngularSpeed )
			.def( "getAngularSpeed", &CollisionMotor::getAngularSpeed )
			;
				
		pybind::interface_<CollisionMotorFollow, pybind::bases<CollisionMotor> >("CollisionMotorFollow")			
			.def( "setTarget", &CollisionMotorFollow::setTarget )
			.def( "getTarget", &CollisionMotorFollow::getTarget )
			.def( "setMinimalDistance", &CollisionMotorFollow::setMinimalDistance )
			.def( "getMinimalDistance", &CollisionMotorFollow::getMinimalDistance )
			;

		pybind::interface_<CollisionMotorPosition, pybind::bases<CollisionMotor> >("CollisionMotorPosition")			
			.def( "setPosition", &CollisionMotorPosition::setPosition )
			;		

		pybind::def_functor( "createCollisionGround", this, &ModuleCollisionGround::createCollisionGround );
		pybind::def_functor( "createCollisionObject", this, &ModuleCollisionGround::createCollisionObject );
		pybind::def_functor( "createCollisionMotorPosition", this, &ModuleCollisionGround::createCollisionMotorPosition );
		pybind::def_functor( "createCollisionMotorFollow", this, &ModuleCollisionGround::createCollisionMotorFollow );
		pybind::def_functor( "createCollisionRadar", this, &ModuleCollisionGround::createCollisionRadar );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleCollisionGround::finalize()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleCollisionGround::setName( const ConstString & _name )
	{
		m_name = _name;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ModuleCollisionGround::getName() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	CollisionGround * ModuleCollisionGround::createCollisionGround()
	{
		CollisionGround * ground = new CollisionGround;

		m_grounds.push_back( ground );

		return ground;
	}
	//////////////////////////////////////////////////////////////////////////
	PythonCollisionObject * ModuleCollisionGround::createCollisionObject( PyObject * _cb )		
	{
		PythonCollisionObject * object = new PythonCollisionObject;
		
		object->setPythonUser( _cb );

		return object;
	}
	//////////////////////////////////////////////////////////////////////////
	CollisionMotorPosition * ModuleCollisionGround::createCollisionMotorPosition()
	{
		CollisionMotorPosition * motor = new CollisionMotorPosition;

		return motor;
	}
	//////////////////////////////////////////////////////////////////////////
	CollisionMotorFollow * ModuleCollisionGround::createCollisionMotorFollow()
	{
		CollisionMotorFollow * motor = new CollisionMotorFollow;

		return motor;
	}
	//////////////////////////////////////////////////////////////////////////
	PythonCollisionRadar * ModuleCollisionGround::createCollisionRadar( PyObject * _pyEnter, PyObject * _pyLeave )
	{
		PythonCollisionRadar * radar = new PythonCollisionRadar;

		radar->setCallback( _pyEnter, _pyLeave );

		return radar;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleCollisionGround::update( float _time, float _timing )
	{
		(void)_time;
		
		for( TVectorCollisionGrounds::iterator 
			it = m_grounds.begin(),
			it_end = m_grounds.end();
		it != it_end;
		++it )
		{
			CollisionGround * ground = *it;

			ground->update( _timing );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleCollisionGround::render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera )
	{

	}
}