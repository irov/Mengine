#	include "ModuleCollision.h"

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/PlayerInterface.h"
#	include "Interface/StringizeInterface.h"

#   include "Kernel/ScriptWrapper.h"
#	include "Kernel/ScriptEventReceiver.h"

#	include "Kernel/NodePrototypeGenerator.h"

#	include "pybind/pybind.hpp"

//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class PythonNodeCollisionActorEventReceiver
		: public ScriptEventReceiver
		, public NodeCollisionActorEventReceiver
	{
	protected:
		bool onNodeCollisionActorCollisionTest(NodeCollisionActor * _other, uint32_t _iff1, uint32_t _iff2, const mt::vec3f & _point, const mt::vec3f & _normal, float _penetration)
		{
			bool result = m_cb.call(_other, _iff1, _iff2, _point, _normal, _penetration);

			return result;
		}
	};
	//////////////////////////////////////////////////////////////////////////
	PyObject * s_NodeCollisionActor_setEventListener(pybind::kernel_interface * _kernel, NodeCollisionActor * _node, PyObject * _args, PyObject * _kwds)
	{
		(void)_args;

		if (_kwds == nullptr)
		{
			return pybind::ret_none();
		}

		pybind::dict py_kwds(_kernel, _kwds);

		Helper::registerScriptEventReceiver<PythonNodeCollisionActorEventReceiver>(py_kwds, _node, "onCollisionTest", EVENT_COLLISION_TEST);

		return pybind::ret_none();
	}
	//////////////////////////////////////////////////////////////////////////
	class PythonNodeCollisionActorUserData
		: public NodeCollisionActorUserData
	{
	public:
		void setObject(const pybind::object & _object)
		{
			m_object = _object;
		}

		const pybind::object & getObject() const
		{
			return m_object;
		}

	protected:
		pybind::object m_object;
	};
	//////////////////////////////////////////////////////////////////////////
	struct extract_NodeCollisionActorUserData
		: public pybind::type_cast_result<NodeCollisionActorUserData *>
	{
		extract_NodeCollisionActorUserData()
		{
            m_factory = new FactoryPool<PythonNodeCollisionActorUserData, 8>();
		}

		bool apply(pybind::kernel_interface * _kernel, PyObject * _obj, TCastValue _value, bool _nothrow) override
		{
            (void)_kernel;
            (void)_nothrow;

			PythonNodeCollisionActorUserData * data = m_factory->createObject();

			data->setObject(pybind::object(_kernel, _obj));

			_value = data;

			return true;
		}

		PyObject * wrap(pybind::kernel_interface * _kernel, TCastRef _value) override
		{
            (void)_kernel;

			const PythonNodeCollisionActorUserData * pyValue = static_cast<const PythonNodeCollisionActorUserData *>(_value);

			const pybind::object & py_object = pyValue->getObject();

			return py_object.ret();
		}

	protected:
		FactoryPtr m_factory;
	};
	//////////////////////////////////////////////////////////////////////////
	ModuleCollision::ModuleCollision()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ModuleCollision::~ModuleCollision()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ModuleCollision::_initialize()
	{
		pybind::kernel_interface * kernel = pybind::get_kernel();

		pybind::interface_<CollisionWorld>( kernel, "CollisionWorld" )
			.def_smart_pointer()
			.def( "setIFFs", &CollisionWorld::setIFFs )
			.def( "getIFFs", &CollisionWorld::getIFFs )
			;

		pybind::def_functor( kernel, "createCollisionWorld", this, &ModuleCollision::createCollisionWorld );
		pybind::def_functor( kernel, "removeCollisionWorld", this, &ModuleCollision::removeCollisionWorld );

		pybind::interface_<NodeCollisionActor, pybind::bases<Node, Eventable> >( kernel, "NodeCollisionActor", false )
			.def( "setCollisionRadius", &NodeCollisionActor::setCollisionRadius )
			.def( "getCollisionRadius", &NodeCollisionActor::getCollisionRadius )
			.def( "setCollisionRaycast", &NodeCollisionActor::setCollisionRaycast )
			.def( "getCollisionRaycast", &NodeCollisionActor::getCollisionRaycast )
			.def( "setCollisionIFF", &NodeCollisionActor::setCollisionIFF )
			.def( "getCollisionIFF", &NodeCollisionActor::getCollisionIFF )
			.def( "setCollisionActive", &NodeCollisionActor::setCollisionActive )
			.def( "getCollisionActive", &NodeCollisionActor::getCollisionActive )
			.def( "setCollisionWorld", &NodeCollisionActor::setCollisionWorld )
			.def( "getCollisionWorld", &NodeCollisionActor::getCollisionWorld )
			.def( "setCollisionUserData", &NodeCollisionActor::setCollisionUserData )
			.def( "getCollisionUserData", &NodeCollisionActor::getCollisionUserData )
			.def( "addCollisionException", &NodeCollisionActor::addCollisionException )
			.def_static_native_kernel("setEventListener", &s_NodeCollisionActor_setEventListener)
			;

		pybind::registration_type_cast<NodeCollisionActorUserData>(kernel, new extract_NodeCollisionActorUserData());

		SCRIPT_SERVICE()
			->setWrapper( Helper::stringizeString( "NodeCollisionActor" ), new ScriptWrapper<NodeCollisionActor>() );

		PROTOTYPE_SERVICE()
			->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "NodeCollisionActor" ), new NodePrototypeGenerator<NodeCollisionActor, 32>() );

        m_factoryCollisionWorld = new FactoryPool<CollisionWorld, 4>();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleCollision::_finalize()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	CollisionWorldPtr ModuleCollision::createCollisionWorld()
	{
		CollisionWorldPtr collision = m_factoryCollisionWorld->createObject();

		if( collision->initialize() == false )
		{
			return nullptr;
		}

		m_collisionWorlds.push_back( collision );

		return collision;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleCollision::removeCollisionWorld( const CollisionWorldPtr & _collision )
	{
		_collision->remove();
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		//////////////////////////////////////////////////////////////////////////
		struct FMagnetDead
		{
			bool operator()( const CollisionWorldPtr & _collision ) const
			{
				if( _collision->isRemoved() == false )
				{
					return false;
				}

				return true;
			}
		};
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleCollision::_tick( float _time, float _timing )
	{
		(void)_time;
		(void)_timing;
		
		for( TVectorCollisionWorlds::iterator
			it = m_collisionWorlds.begin(),
			it_end = m_collisionWorlds.end();
		it != it_end;
		++it )
		{
			const CollisionWorldPtr & collision = *it;

			collision->update();
		}

		TVectorCollisionWorlds::iterator it_erase = std::remove_if( m_collisionWorlds.begin(), m_collisionWorlds.end(), FMagnetDead() );
		m_collisionWorlds.erase( it_erase, m_collisionWorlds.end() );
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleCollision::_render( const RenderObjectState * _state, uint32_t _debugMask )
	{
		(void)_state;
		(void)_debugMask;
	}
}