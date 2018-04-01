#include "Box2DModule.h"
#include "Box2DWorld.h"
#include "Box2DJoint.h"
#include "NodeBox2DBody.h"

#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/ScriptablePrototypeGenerator.h"
#include "Kernel/ScriptEventReceiver.h"

#include "Kernel/ScriptWrapper.h"

#include <algorithm>

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
	Box2DModule::Box2DModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
	Box2DModule::~Box2DModule()
    {
    }
	//////////////////////////////////////////////////////////////////////////
	namespace 
	{
		class PythonBox2DBodyEventReceiver
			: public ScriptEventReceiver
			, public Box2DBodyEventReceiver
		{
		public:
			void onBox2DBodyBeginContact( class Box2DBody * _other, b2Contact * _contact ) override
			{
				m_cb.call( _other, _contact );
			}

			void onBox2DBodyEndContact( class Box2DBody * _other, b2Contact * _contact ) override
			{
				m_cb.call( _other, _contact );
			}

			void onBox2DBodyPreSolve( class Box2DBody * _other, b2Contact * _contact, const b2Manifold* _manifold ) override
			{
				m_cb.call( _other, _contact, _manifold );
			}

			void onBox2DBodyPostSolve( class Box2DBody * _other, b2Contact * _contact, const b2ContactImpulse* _impulse ) override
			{
				m_cb.call( _other, _contact, _impulse );
			}

			void onBox2DBodyUpdateContact( class Box2DBody * _other, b2Contact * _contact )
			{
				m_cb.call( _other, _contact );
			}
		};
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Box2DModule::s_Box2DBody_setEventListener( pybind::kernel_interface * _kernel, Box2DBody * _node, PyObject * _args, PyObject * _kwds )
	{
		(void)_args;

		if( _kwds == nullptr )
		{
			return pybind::ret_none();
		}

		pybind::dict py_kwds( _kernel, _kwds );
		Helper::registerScriptEventReceiver<PythonBox2DBodyEventReceiver>( py_kwds, _node, "onBox2DBodyBeginContact", EVENT_BOX2DBODY_BEGIN_CONTACT );
		Helper::registerScriptEventReceiver<PythonBox2DBodyEventReceiver>( py_kwds, _node, "onBox2DBodyEndContact", EVENT_BOX2DBODY_END_CONTACT );
		Helper::registerScriptEventReceiver<PythonBox2DBodyEventReceiver>( py_kwds, _node, "onBox2DBodyPreSolve", EVENT_BOX2DBODY_PRE_SOLVE );
		Helper::registerScriptEventReceiver<PythonBox2DBodyEventReceiver>( py_kwds, _node, "onBox2DBodyPostSolve", EVENT_BOX2DBODY_POST_SOLVE );

#ifdef _DEBUG
		if( py_kwds.empty() == false )
		{
			for( pybind::dict::iterator
				it = py_kwds.begin(),
				it_end = py_kwds.end();
				it != it_end;
				++it )
			{
				std::string k = it.key();

				LOGGER_ERROR("setEventListener invalid kwds '%s'\n"
					, k.c_str()
					);
			}

			throw;
		}
#endif

		return pybind::ret_none();
	}
	//////////////////////////////////////////////////////////////////////////
	static float get_b2ContactImpulse_normalImpulses( pybind::kernel_interface * _kernel, b2ContactImpulse * _impulse, uint32_t i )
	{
		(void)_kernel;

		return _impulse->normalImpulses[i];
	}
	//////////////////////////////////////////////////////////////////////////
	static float get_b2ContactImpulse_tangentImpulses( pybind::kernel_interface * _kernel, b2ContactImpulse * _impulse, uint32_t i )
	{
		(void)_kernel;

		return _impulse->tangentImpulses[i];
	}
	//////////////////////////////////////////////////////////////////////////
	static b2ManifoldPoint get_b2Manifold_points( pybind::kernel_interface * _kernel, b2Manifold * m, uint32_t i )
	{
		(void)_kernel;

		const b2ManifoldPoint & p = m->points[i];

		return p;
	}
	//////////////////////////////////////////////////////////////////////////
	static b2Shape * get_b2Fixture_GetShape( pybind::kernel_interface * _kernel, b2Fixture * f )
	{
		(void)_kernel;

		return f->GetShape();
	}
	//////////////////////////////////////////////////////////////////////////
	static b2Body * get_b2Fixture_GetBody( pybind::kernel_interface * _kernel, b2Fixture * f )
	{
		(void)_kernel;

		return f->GetBody();
	}
	//////////////////////////////////////////////////////////////////////////
	static b2Fixture * get_b2Contact_GetFixtureA( pybind::kernel_interface * _kernel, b2Contact * _contact )
	{
		(void)_kernel;

		return _contact->GetFixtureA();
	}
	//////////////////////////////////////////////////////////////////////////
	static b2Fixture * get_b2Contact_GetFixtureB( pybind::kernel_interface * _kernel, b2Contact * _contact )
	{
		(void)_kernel;

		return _contact->GetFixtureB();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Box2DModule::_initialize()
    {
		pybind::kernel_interface * kernel = pybind::get_kernel();

		pybind::def_functor_args( kernel, "createBox2DWorld", this, &Box2DModule::createWorld );
		pybind::def_functor( kernel, "destroyBox2DWorld", this, &Box2DModule::destroyWorld );

		pybind::interface_<Box2DWorld, pybind::bases<Scriptable> >( kernel, "Box2DWorld" )
			.def_smart_pointer()
			.def_bindable()
			.def( "setTimeStep", &Box2DWorld::setTimeStep )
			.def( "createBody", &Box2DWorld::createBody )
			.def( "createDistanceJoint", &Box2DWorld::createDistanceJoint )
			.def( "createHingeJoint", &Box2DWorld::createHingeJoint )
			.def( "createPrismaticJoint", &Box2DWorld::createPrismaticJoint )
			.def( "createPulleyJoint", &Box2DWorld::createPulleyJoint )
			.def( "createGearJoint", &Box2DWorld::createGearJoint )
			.def( "createRopeJoint", &Box2DWorld::createRopeJoint )
			.def( "createWheelJoint", &Box2DWorld::createWheelJoint )
			.def( "rayCast", &Box2DWorld::rayCast )
			;
			
		pybind::interface_<Box2DBody, pybind::bases<Scriptable, Eventable> >( kernel, "Box2DBody" )
			.def_smart_pointer()
			.def_bindable()
			.def( "setUserData", &Box2DBody::setUserData )
			.def( "getUserData", &Box2DBody::getUserData )
			.def( "addShapeConvex", &Box2DBody::addShapeConvex )
			.def( "addShapeCircle", &Box2DBody::addShapeCircle )
			.def( "addShapeBox", &Box2DBody::addShapeBox )
			.def( "getMass", &Box2DBody::getMass )
			.def( "getInertia", &Box2DBody::getInertia )
			.def( "setLinearVelocity", &Box2DBody::setLinearVelocity )
			.def( "getLinearVelocity", &Box2DBody::getLinearVelocity )
			.def( "setAngularVelocity", &Box2DBody::setAngularVelocity )
			.def( "getAngularVelocity", &Box2DBody::getAngularVelocity )
			.def( "applyForce", &Box2DBody::applyForce )
			.def( "applyImpulse", &Box2DBody::applyImpulse )
			.def( "applyTorque", &Box2DBody::applyTorque )
			.def( "isFrozen", &Box2DBody::isFrozen )
			.def( "isSleeping", &Box2DBody::isSleeping )
			.def( "isStatic", &Box2DBody::isStatic )
			.def( "setLinearDumping", &Box2DBody::setLinearDumping )
			.def( "getLinearDumping", &Box2DBody::getLinearDumping )
			.def( "setAngularDumping", &Box2DBody::setAngularDumping )
			.def( "getAngularDumping", &Box2DBody::getAngularDumping )
			.def( "setFixedRotation", &Box2DBody::setFixedRotation )
			.def( "getFixedRotation", &Box2DBody::getFixedRotation )
			.def( "setIsBullet", &Box2DBody::setIsBullet )
			.def( "getIsBullet", &Box2DBody::getIsBullet )
			.def( "sleep", &Box2DBody::sleep )
			.def( "wakeUp", &Box2DBody::wakeUp )
			.def_proxy_native_kernel( "setEventListener", this, &Box2DModule::s_Box2DBody_setEventListener )
			;

		pybind::interface_<Box2DJoint, pybind::bases<Scriptable> >( kernel, "Box2DJoint" )
			.def_smart_pointer()
			.def_bindable()
			;

		pybind::struct_<b2Vec2>( kernel, "b2Vec2" )
			.def_constructor( pybind::init<float, float>() )
			.def_member( "x", &b2Vec2::x )
			.def_member( "y", &b2Vec2::y )
			;

		pybind::struct_<b2Vec3>( kernel, "b2Vec3" )
			.def_constructor( pybind::init<float, float, float>() )
			.def_member( "x", &b2Vec3::x )
			.def_member( "y", &b2Vec3::y )
			.def_member( "z", &b2Vec3::z )
			;

		pybind::struct_<b2ContactImpulse>( kernel, "b2ContactImpulse" )
			.def_static_kernel( "normalImpulses", &get_b2ContactImpulse_normalImpulses )
			.def_static_kernel( "tangentImpulses", &get_b2ContactImpulse_tangentImpulses )
			.def_member( "count", &b2ContactImpulse::count )
			;

		pybind::struct_<b2ManifoldPoint>( kernel, "b2ManifoldPoint" )
			.def_member( "localPoint", &b2ManifoldPoint::localPoint )
			.def_member( "normalImpulse", &b2ManifoldPoint::normalImpulse )
			.def_member( "tangentImpulse", &b2ManifoldPoint::tangentImpulse )
			.def_member( "id", &b2ManifoldPoint::id )
			;

		pybind::interface_<b2Manifold>( kernel, "b2Manifold" )
			.def_static_kernel( "points", &get_b2Manifold_points )
			.def_member( "localNormal", &b2Manifold::localNormal )
			.def_member( "localPoint", &b2Manifold::localPoint )
			.def_member( "pointCount", &b2Manifold::pointCount )
			;

		pybind::interface_<b2Shape>( kernel, "b2Shape" )
			;

		pybind::interface_<b2Fixture>( kernel, "b2Fixture" )
			.def_static_kernel( "GetShape", &get_b2Fixture_GetShape )
			.def( "SetSensor", &b2Fixture::SetSensor )
			.def( "IsSensor", &b2Fixture::IsSensor )
			.def_static_kernel( "GetBody", &get_b2Fixture_GetBody )
			.def( "TestPoint", &b2Fixture::TestPoint )
			.def( "SetDensity", &b2Fixture::SetDensity )
			.def( "GetDensity", &b2Fixture::GetDensity )
			.def( "GetFriction", &b2Fixture::GetFriction )
			.def( "SetFriction", &b2Fixture::SetFriction )
			.def( "GetRestitution", &b2Fixture::GetRestitution )
			.def( "SetRestitution", &b2Fixture::SetRestitution )
			;

		pybind::interface_<b2Contact>( kernel, "b2Contact" )
			.def( "IsTouching", &b2Contact::IsTouching )
			.def( "SetEnabled", &b2Contact::SetEnabled )
			.def( "IsEnabled", &b2Contact::IsEnabled )
			.def_static_kernel( "GetFixtureA", &get_b2Contact_GetFixtureA )
			.def_static_kernel( "GetFixtureB", &get_b2Contact_GetFixtureB )
			.def( "SetFriction", &b2Contact::SetFriction )
			.def( "GetFriction", &b2Contact::GetFriction )
			.def( "ResetFriction", &b2Contact::ResetFriction )
			.def( "SetRestitution", &b2Contact::SetRestitution )
			.def( "GetRestitution", &b2Contact::GetRestitution )
			.def( "ResetRestitution", &b2Contact::ResetRestitution )
			.def( "SetTangentSpeed", &b2Contact::SetTangentSpeed )
			.def( "GetTangentSpeed", &b2Contact::GetTangentSpeed )
			;

		pybind::interface_<NodeBox2DBody, pybind::bases<Node> >( kernel, "NodeBox2DBody" )
			.def( "setBox2DBody", &NodeBox2DBody::setBox2DBody )
			.def( "getBox2DBody", &NodeBox2DBody::getBox2DBody )
			;

		SCRIPT_SERVICE()
			->setWrapper( STRINGIZE_STRING_LOCAL( "NodeBox2DBody" ), new ScriptWrapper<NodeBox2DBody>() );

		SCRIPT_SERVICE()
			->setWrapper( STRINGIZE_STRING_LOCAL( "Box2DBody" ), new ScriptWrapper<Box2DBody>() );

		SCRIPT_SERVICE()
			->setWrapper( STRINGIZE_STRING_LOCAL( "Box2DJoint" ), new ScriptWrapper<Box2DJoint>() );

		SCRIPT_SERVICE()
			->setWrapper( STRINGIZE_STRING_LOCAL( "Box2DWorld" ), new ScriptWrapper<Box2DWorld>() );

		if( PROTOTYPE_SERVICE()
			->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "NodeBox2DBody" ), new NodePrototypeGenerator<NodeBox2DBody, 128> ) == false )
		{
			return false;
		}

		if( PROTOTYPE_SERVICE()
			->addPrototype( STRINGIZE_STRING_LOCAL( "Box2D" ), STRINGIZE_STRING_LOCAL( "Box2DBody" ), new ScriptablePrototypeGenerator<Box2DBody, 128> ) == false )
		{
			return false;
		}

		if( PROTOTYPE_SERVICE()
			->addPrototype( STRINGIZE_STRING_LOCAL( "Box2D" ), STRINGIZE_STRING_LOCAL( "Box2DJoint" ), new ScriptablePrototypeGenerator<Box2DJoint, 128> ) == false )
		{
			return false;
		}

		if( PROTOTYPE_SERVICE()
			->addPrototype( STRINGIZE_STRING_LOCAL( "Box2D" ), STRINGIZE_STRING_LOCAL( "Box2DWorld" ), new ScriptablePrototypeGenerator<Box2DWorld, 128> ) == false )
		{
			return false;
		}
		
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
	void Box2DModule::_finalize()
    {
		for( TVectorWorlds::iterator
			it = m_worlds.begin(),
			it_end = m_worlds.end();
		it != it_end;
		++it )
		{
			Box2DWorld * world = *it;

			world->finalize();

			delete world;
		}

		m_worlds.clear();
    }
    //////////////////////////////////////////////////////////////////////////
	Box2DWorld * Box2DModule::createWorld( const mt::vec2f& _gravity, const pybind::object & _update, const pybind::args & _update_args )
    {
		Box2DWorld * world = PROTOTYPE_SERVICE()
			->generatePrototype( STRINGIZE_STRING_LOCAL( "Box2D" ), STRINGIZE_STRING_LOCAL( "Box2DWorld" ) );

		if( world->initialize( _gravity, _update, _update_args ) == false )
		{
			return nullptr;
		}

		m_worldsAdd.push_back( world );

		return world;
    }
	//////////////////////////////////////////////////////////////////////////
	void Box2DModule::destroyWorld( Box2DWorld * _world )
	{
		_world->setDead();
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		struct FWorldDead
		{
			bool operator ()( Box2DWorld * _event ) const
			{
				return _event->isDead();
			}
		};
	}
    //////////////////////////////////////////////////////////////////////////
	void Box2DModule::_tick( float _time, float _timing )
    {
		m_worlds.insert( m_worlds.end(), m_worldsAdd.begin(), m_worldsAdd.end() );
		m_worldsAdd.clear();

		for( TVectorWorlds::iterator
			it = m_worlds.begin(),
			it_end = m_worlds.end();
		it != it_end;
		++it )
		{
			Box2DWorld * world = *it;

			if( world->isDead() == true )
			{
				continue;
			}

			world->update( _time, _timing );
		}

		TVectorWorlds::iterator it_erase = std::remove_if( m_worlds.begin(), m_worlds.end(), FWorldDead() );
		m_worlds.erase( it_erase, m_worlds.end() );
    }
	//////////////////////////////////////////////////////////////////////////
	void Box2DModule::_render( const RenderState * _state, uint32_t _debugMask )
	{
		(void)_state;
		(void)_debugMask;
		//Empty
	}
    //////////////////////////////////////////////////////////////////////////
}