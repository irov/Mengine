#	include "Box2DModule.h"
#	include "Box2DWorld.h"
#	include "Box2DJoint.h"
#	include "NodeBox2DBody.h"

#	include "Kernel/NodePrototypeGenerator.h"
#	include "Kernel/ScriptablePrototypeGenerator.h"
#	include "Kernel/ScriptEventReceiver.h"

#   include "Kernel/ScriptWrapper.h"

#	include <algorithm>

//////////////////////////////////////////////////////////////////////////
namespace Menge
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
			void onBox2DBodyBeginContact( class Box2DBody * _other, const mt::vec2f & _position, const mt::vec2f & _normal ) override
			{
				m_cb.call( _other, _position, _normal );
			}

			void onBox2DBodyEndContact( class Box2DBody * _other ) override
			{
				m_cb.call( _other );
			}

			void onBox2DBodyPreSolve( class Box2DBody * _other, const mt::vec2f & _position, const mt::vec2f & _normal ) override
			{
				m_cb.call( _other, _position, _normal );
			}

			void onBox2DBodyPostSolve( class Box2DBody * _other, const mt::vec2f & _position, const mt::vec2f & _normal ) override
			{
				m_cb.call( _other, _position, _normal );
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

#	ifdef _DEBUG
		if( py_kwds.empty() == false )
		{
			for( pybind::dict::iterator
				it = py_kwds.begin(),
				it_end = py_kwds.end();
				it != it_end;
				++it )
			{
				std::string k = it.key();

				LOGGER_ERROR( m_serviceProvider )("setEventListener invalid kwds '%s'\n"
					, k.c_str()
					);
			}

			throw;
		}
#	endif

		return pybind::ret_none();
	}
    //////////////////////////////////////////////////////////////////////////
	bool Box2DModule::_initialize()
    {
		pybind::kernel_interface * kernel = pybind::get_kernel();

		pybind::def_functor( kernel, "createBox2DWorld", this, &Box2DModule::createWorld );
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

		pybind::interface_<NodeBox2DBody, pybind::bases<Node> >( kernel, "NodeBox2DBody" )
			.def( "setBox2DBody", &NodeBox2DBody::setBox2DBody )
			.def( "getBox2DBody", &NodeBox2DBody::getBox2DBody )
			;

		SCRIPT_SERVICE( m_serviceProvider )
			->setWrapper( STRINGIZE_STRING_LOCAL( m_serviceProvider, "NodeBox2DBody" ), new ScriptWrapper<NodeBox2DBody>() );

		SCRIPT_SERVICE( m_serviceProvider )
			->setWrapper( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Box2DBody" ), new ScriptWrapper<Box2DBody>() );

		SCRIPT_SERVICE( m_serviceProvider )
			->setWrapper( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Box2DJoint" ), new ScriptWrapper<Box2DJoint>() );

		SCRIPT_SERVICE( m_serviceProvider )
			->setWrapper( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Box2DWorld" ), new ScriptWrapper<Box2DWorld>() );

		if( PROTOTYPE_SERVICE( m_serviceProvider )
			->addPrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Node" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "NodeBox2DBody" ), new NodePrototypeGenerator<NodeBox2DBody, 128> ) == false )
		{
			return false;
		}

		if( PROTOTYPE_SERVICE( m_serviceProvider )
			->addPrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Box2D" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "Box2DBody" ), new ScriptablePrototypeGenerator<Box2DBody, 128> ) == false )
		{
			return false;
		}

		if( PROTOTYPE_SERVICE( m_serviceProvider )
			->addPrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Box2D" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "Box2DJoint" ), new ScriptablePrototypeGenerator<Box2DJoint, 128> ) == false )
		{
			return false;
		}

		if( PROTOTYPE_SERVICE( m_serviceProvider )
			->addPrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Box2D" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "Box2DWorld" ), new ScriptablePrototypeGenerator<Box2DWorld, 128> ) == false )
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
	Box2DWorld * Box2DModule::createWorld( const mt::vec2f& _gravity )
    {
		Box2DWorld * world = PROTOTYPE_SERVICE( m_serviceProvider )
			->generatePrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Box2D" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "Box2DWorld" ) );

		if( world->initialize( _gravity ) == false )
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
	void Box2DModule::_render( const RenderObjectState * _state, uint32_t _debugMask )
	{
		(void)_state;
		(void)_debugMask;
		//Empty
	}
    //////////////////////////////////////////////////////////////////////////
}