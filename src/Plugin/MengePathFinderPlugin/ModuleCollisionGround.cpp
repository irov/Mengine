#	include "ModuleCollisionGround.h"

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/PlayerInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Kernel/NodePrototypeGenerator.h"
#	include "Kernel/ScriptClassWrapper.h"

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

		pybind::interface_<CollisionObject, pybind::bases<Node> >("CollisionObject")			
			.def( "setRadius", &CollisionObject::setRadius )
			.def( "getRadius", &CollisionObject::getRadius )
			.def( "setMass", &CollisionObject::setMass )
			.def( "getMass", &CollisionObject::getMass )
			.def( "setMotor", &CollisionObject::setMotor )
			.def( "getMotor", &CollisionObject::getMotor )
			.def( "setDead", &CollisionObject::setDead )
			.def( "getDead", &CollisionObject::getDead )
			.def( "setGhost", &CollisionObject::setGhost )
			.def( "getGhost", &CollisionObject::getGhost )			
			;

		pybind::interface_<PythonCollisionObject, pybind::bases<CollisionObject> >("PythonCollisionObject")		
			.def( "setPythonUser", &PythonCollisionObject::setPythonUser )
			.def( "getPythonUser", &PythonCollisionObject::getPythonUser )
			;
		
		pybind::interface_<CollisionRadar, pybind::bases<Node> >("CollisionRadar")	
			.def( "setRadius", &CollisionRadar::setRadius )
			.def( "getRadius", &CollisionRadar::getRadius )
			.def( "setExceptCollisionObject", &CollisionRadar::setExceptCollisionObject )
			.def( "getExceptCollisionObject", &CollisionRadar::getExceptCollisionObject )			
			.def( "inside", &CollisionRadar::inside )
			.def( "setDead", &CollisionRadar::setDead )
			.def( "getDead", &CollisionRadar::getDead )
			;

		pybind::interface_<PythonCollisionRadar, pybind::bases<CollisionRadar> >("PythonCollisionRadar")
			.def( "setCallback", &PythonCollisionRadar::setCallback )
			.def( "findMinimalObject", &PythonCollisionRadar::findMinimalObject )
			;

		pybind::interface_<CollisionMotor>("CollisionMotor")			
			.def( "setLinearSpeed", &CollisionMotor::setLinearSpeed )
			.def( "getLinearSpeed", &CollisionMotor::getLinearSpeed )
			.def( "setAngularSpeed", &CollisionMotor::setAngularSpeed )
			.def( "getAngularSpeed", &CollisionMotor::getAngularSpeed )
			.def( "setMoveStop", &CollisionMotor::setMoveStop )
			.def( "getMoveStop", &CollisionMotor::getMoveStop )
			;
				
		pybind::interface_<CollisionMotorFollow, pybind::bases<CollisionMotor> >("CollisionMotorFollow")			
			.def( "setTarget", &CollisionMotorFollow::setTarget )
			.def( "getTarget", &CollisionMotorFollow::getTarget )
			.def( "setMinimalDistance", &CollisionMotorFollow::setMinimalDistance )
			.def( "getMinimalDistance", &CollisionMotorFollow::getMinimalDistance )
			;

		pybind::interface_<CollisionMotorPosition, pybind::bases<CollisionMotor> >("CollisionMotorPosition")			
			.def( "setPositionAngle", &CollisionMotorPosition::setPositionAngle )
			.def( "setPositionLookAt", &CollisionMotorPosition::setPositionLookAt )
			;		

		pybind::def_functor( "createCollisionGround", this, &ModuleCollisionGround::createCollisionGround );
		pybind::def_functor( "createCollisionMotorPosition", this, &ModuleCollisionGround::createCollisionMotorPosition );
		pybind::def_functor( "createCollisionMotorFollow", this, &ModuleCollisionGround::createCollisionMotorFollow );
		
		PROTOTYPE_SERVICE(m_serviceProvider)
			->addPrototype( CONST_STRING_LOCAL(m_serviceProvider, "Node"), CONST_STRING_LOCAL( m_serviceProvider, "PythonCollisionObject"), new NodePrototypeGenerator<PythonCollisionObject, 128>(m_serviceProvider) );

		PROTOTYPE_SERVICE(m_serviceProvider)
			->addPrototype( CONST_STRING_LOCAL(m_serviceProvider, "Node"), CONST_STRING_LOCAL( m_serviceProvider, "PythonCollisionRadar"), new NodePrototypeGenerator<PythonCollisionRadar, 128>(m_serviceProvider) );

		SCRIPT_SERVICE(m_serviceProvider)
			->addWrapping( CONST_STRING_LOCAL(m_serviceProvider, "PythonCollisionObject"), new ScriptClassWrapper<PythonCollisionObject>() );

		SCRIPT_SERVICE(m_serviceProvider)
			->addWrapping( CONST_STRING_LOCAL(m_serviceProvider, "PythonCollisionRadar"), new ScriptClassWrapper<PythonCollisionRadar>() );

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
	CollisionMotorPosition * ModuleCollisionGround::createCollisionMotorPosition( PyObject * _cb )
	{
		CollisionMotorPosition * motor = new CollisionMotorPosition;

		motor->setCallback( _cb );

		return motor;
	}
	//////////////////////////////////////////////////////////////////////////
	CollisionMotorFollow * ModuleCollisionGround::createCollisionMotorFollow( PyObject * _cb )
	{
		CollisionMotorFollow * motor = new CollisionMotorFollow;

		motor->setCallback( _cb );

		return motor;
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
	void ModuleCollisionGround::render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, unsigned int _debugMask )
	{

	}
}