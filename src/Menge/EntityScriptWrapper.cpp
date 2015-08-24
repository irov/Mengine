#	include "ScriptWrapper.h"

#	include "Kernel/ScriptClassWrapper.h"

#	include "Kernel/Entity.h"
#	include "Kernel/Scene.h"

#	include "Arrow.h"

#   include "Interface/ResourceInterface.h"
#   include "Interface/ScriptSystemInterface.h"
#   include "Interface/NodeInterface.h"

#   include "Consts.h"
#	include "Logger/Logger.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	class EntityScriptMethod
	{
    public:
        EntityScriptMethod( ServiceProviderInterface * _serviceProvider )
            : m_serviceProvider(_serviceProvider)
        {
        }

    public:
        //////////////////////////////////////////////////////////////////////////
        bool s_addPrototypeFinder( const ConstString & _category, const ConstString & _prototype, const pybind::object & _module )
        {
            PrototypeGeneratorInterfacePtr generator = SCRIPT_SERVICE(m_serviceProvider)
				->createEntityGenerator(_category, _prototype, _module);

            bool successful = PROTOTYPE_SERVICE(m_serviceProvider)
                ->addPrototype( _category, _prototype, generator );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
		bool s_addEntityPrototypeFinder( const ConstString & _prototype, const pybind::object & _module )
        {
            bool result = s_addPrototypeFinder( CONST_STRING(m_serviceProvider, Entity), _prototype, _module );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
		bool s_addScenePrototypeFinder( const ConstString & _prototype, const pybind::object & _module )
        {
            bool result = s_addPrototypeFinder( CONST_STRING(m_serviceProvider, Scene), _prototype, _module );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
		bool s_addArrowPrototypeFinder( const ConstString & _prototype, const pybind::object & _module )
        {
            bool result = s_addPrototypeFinder( CONST_STRING(m_serviceProvider, Arrow), _prototype, _module );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
		const pybind::object & s_createEntity( const ConstString & _prototype )
		{
			Entity * entity = PROTOTYPE_SERVICE(m_serviceProvider)
				->generatePrototypeT<Entity>( CONST_STRING(m_serviceProvider, Entity), _prototype );

			if( entity == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("Error: can't create Entity '%s'"
					, _prototype.c_str()
					);

				return pybind::object::get_invalid();
			}

			NODE_SERVICE( m_serviceProvider )
				->addHomeless( entity );

			const pybind::object & py_entity = entity->getScriptObject();

			return py_entity;
		}
        //////////////////////////////////////////////////////////////////////////
		pybind::object s_importEntity( const ConstString & _prototype )
        {
			pybind::object py_type = SCRIPT_SERVICE( m_serviceProvider )
				->importEntity( CONST_STRING(m_serviceProvider, Entity), _prototype );
			
			if( py_type.is_invalid() == true )
			{
				return pybind::make_none_t();
			}
			
			return py_type;
        }

    protected:
        ServiceProviderInterface * m_serviceProvider;
	};
    //////////////////////////////////////////////////////////////////////////
	static void classWrapping( ServiceProviderInterface * _serviceProvider )
	{
		SCRIPT_CLASS_WRAPPING( _serviceProvider, Entity );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, Scene );
	}
    //////////////////////////////////////////////////////////////////////////
	class superclass_new_Entity
		: public pybind::new_adapter_interface
	{
	public:
		void * call( const pybind::class_type_scope_ptr & _scope, PyObject * _obj, PyObject * _args, PyObject * _kwds ) override
		{
			(void)_obj;
			(void)_args;
			(void)_kwds;

			ServiceProviderInterface * serviceProvider = _scope->get_user_t<ServiceProviderInterface>();

			Entity * entity = NODE_SERVICE( serviceProvider )
				->createNodeT<Entity>( CONST_STRING( serviceProvider, Entity ) );

			entity->setEmbed( _obj );

			pybind::decref( _obj );

			return entity;
		}
	};
    //////////////////////////////////////////////////////////////////////////
	class superclass_new_Arrow
		: public pybind::new_adapter_interface
	{
	public:
		void * call( const pybind::class_type_scope_ptr & _scope, PyObject * _obj, PyObject * _args, PyObject * _kwds ) override
		{
			(void)_obj;
			(void)_args;
			(void)_kwds;

			ServiceProviderInterface * serviceProvider = _scope->get_user_t<ServiceProviderInterface>();

			Arrow * arrow = NODE_SERVICE( serviceProvider )
				->createNodeT<Arrow>( CONST_STRING( serviceProvider, Arrow ) );

			arrow->setEmbed( _obj );

			pybind::decref( _obj );

			return arrow;
		}
	};
    //////////////////////////////////////////////////////////////////////////
	class superclass_new_Scene
		: public pybind::new_adapter_interface
	{
	public:
		void * call( const pybind::class_type_scope_ptr & _scope, PyObject * _obj, PyObject * _args, PyObject * _kwds ) override
		{
			(void)_obj;
			(void)_args;
			(void)_kwds;

			ServiceProviderInterface * serviceProvider = _scope->get_user_t<ServiceProviderInterface>();

			Scene * scene = NODE_SERVICE( serviceProvider )
				->createNodeT<Scene>( CONST_STRING( serviceProvider, Scene ) );

			scene->setEmbed( _obj );

			pybind::decref( _obj );

			return scene;
		}
	};
	//////////////////////////////////////////////////////////////////////////
	void ScriptWrapper::entityWrap( ServiceProviderInterface * _serviceProvider )
	{
		classWrapping( _serviceProvider );

		pybind::superclass_<Entity, pybind::bases<Node> >("Entity", (void *)_serviceProvider, new superclass_new_Entity, nullptr, false)
            .def_constructor( pybind::init<>() )
			.def( "getPrototype", &Entity::getPrototype )
			;

		pybind::superclass_<Arrow, pybind::bases<Entity> >( "Arrow", (void *)_serviceProvider, new superclass_new_Arrow, nullptr, false )
            .def_constructor( pybind::init<>() )
			.def( "setOffsetClick", &Arrow::setOffsetClick )
			.def( "getOffsetClick", &Arrow::getOffsetClick )
			.def( "setPolygon", &Arrow::setPolygon )
			.def( "getPolygon", &Arrow::getPolygon )
			.def( "setRadius", &Arrow::setRadius )
			.def( "getRadius", &Arrow::getRadius )
			;

		pybind::superclass_<Scene, pybind::bases<Entity> >("Scene", (void *)_serviceProvider, new superclass_new_Scene, nullptr, false)
            .def_constructor( pybind::init<>() )
			.def( "isSubScene", &Scene::isSubScene )
			.def( "getParentScene", &Scene::getParentScene )
			//.def( "setRenderTarget", &Scene::setRenderTarget )
			//.def( "renderSelf", &Scene::renderSelf )
			//.def( "blockInput", &Scene::blockInput )
			//.def( "getBlockInput", &Scene::getBlockInput )
			.def( "getMainLayer", &Scene::getMainLayer )
			.def( "setMainLayer", &Scene::setMainLayer )
			;

        EntityScriptMethod * entityScriptMethod = new EntityScriptMethod(_serviceProvider);

        pybind::def_functor( "addEntityPrototypeFinder", entityScriptMethod, &EntityScriptMethod::s_addEntityPrototypeFinder );
        pybind::def_functor( "addScenePrototypeFinder", entityScriptMethod, &EntityScriptMethod::s_addScenePrototypeFinder );
        pybind::def_functor( "addArrowPrototypeFinder", entityScriptMethod, &EntityScriptMethod::s_addArrowPrototypeFinder );
		pybind::def_functor( "createEntity", entityScriptMethod, &EntityScriptMethod::s_createEntity );
        pybind::def_functor( "importEntity", entityScriptMethod, &EntityScriptMethod::s_importEntity );
		//pybind::def_function( "createEntityFromBinary", &ScriptMethod::createEntityFromBinary );
	}
}
