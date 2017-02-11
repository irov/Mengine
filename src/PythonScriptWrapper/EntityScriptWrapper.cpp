#	include "PythonScriptWrapper.h"

#	include "PythonScriptWrapper/ScriptClassWrapper.h"

#	include "Kernel/Entity.h"
#	include "Kernel/Scene.h"

#	include "Kernel/Arrow.h"

#   include "Interface/ResourceInterface.h"
#   include "Interface/ScriptSystemInterface.h"
#   include "Interface/NodeInterface.h"
#	include "EntityPrototypeGenerator.h"

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
        bool s_addPrototypeFinder( const ConstString & _category, const ConstString & _prototype, const pybind::object & _generator)
        {
            EntityPrototypeGeneratorPtr generator = m_factoryEntityPrototypeGenerator.createObject();

            generator->setScriptGenerator(_generator);

			bool successful = PROTOTYPE_SERVICE( m_serviceProvider )
				->addPrototype( _category, _prototype, generator);

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
		bool s_addEntityPrototypeFinder( const ConstString & _prototype, const pybind::object & _module )
        {
            bool result = s_addPrototypeFinder(STRINGIZE_STRING_LOCAL(m_serviceProvider, "Entity"), _prototype, _module );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
		bool s_addScenePrototypeFinder( const ConstString & _prototype, const pybind::object & _module )
        {
            bool result = s_addPrototypeFinder( STRINGIZE_STRING_LOCAL(m_serviceProvider, "Scene"), _prototype, _module );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
		bool s_addArrowPrototypeFinder( const ConstString & _prototype, const pybind::object & _module )
        {
            bool result = s_addPrototypeFinder(STRINGIZE_STRING_LOCAL(m_serviceProvider, "Arrow"), _prototype, _module );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
		const pybind::object & s_createEntity( const ConstString & _prototype )
		{
			Entity * entity = PROTOTYPE_SERVICE(m_serviceProvider)
				->generatePrototypeT<Entity *>(STRINGIZE_STRING_LOCAL(m_serviceProvider, "Entity"), _prototype );

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
		pybind::object s_importEntity( pybind::kernel_interface * _kernel, const ConstString & _prototype )
        {
			(void)_kernel;

            PrototypeGeneratorInterfacePtr generator;
            if( PROTOTYPE_SERVICE( m_serviceProvider )
                ->hasPrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Entity" ), _prototype, generator ) == false )
            {
                LOGGER_ERROR( m_serviceProvider )("importEntity: can't import 'Entity' '%s'"                    
                    , _prototype.c_str()
                    );

				return pybind::make_none_t( _kernel );
            }

            EntityPrototypeGeneratorPtr entityGenerator =
                stdex::intrusive_static_cast<EntityPrototypeGeneratorPtr>(generator);

            pybind::object py_type = entityGenerator->preparePythonType();
			
			return py_type;
        }

    protected:
        typedef FactoryPoolStore<EntityPrototypeGenerator, 64> FactoryEntityPrototypeGenerator;
        FactoryEntityPrototypeGenerator m_factoryEntityPrototypeGenerator;

        ServiceProviderInterface * m_serviceProvider;
	};
    //////////////////////////////////////////////////////////////////////////
	static void classWrapping( ServiceProviderInterface * _serviceProvider )
	{
# define SCRIPT_CLASS_WRAPPING( serviceProvider, Class )\
    SCRIPT_SERVICE(serviceProvider)->setWrapper( Helper::stringizeString(serviceProvider, #Class), new ClassScriptWrapper<Class>() )

		SCRIPT_CLASS_WRAPPING( _serviceProvider, Entity );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, Scene );

#	undef SCRIPT_CLASS_WRAPPING
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
				->createNodeT<Entity *>(STRINGIZE_STRING_LOCAL( serviceProvider, "Entity" ) );

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
				->createNodeT<Arrow *>(STRINGIZE_STRING_LOCAL( serviceProvider, "Arrow" ) );

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
				->createNodeT<Scene *>(STRINGIZE_STRING_LOCAL( serviceProvider, "Scene" ) );

			scene->setEmbed( _obj );

			pybind::decref( _obj );

			return scene;
		}
	};
	//////////////////////////////////////////////////////////////////////////
	void PythonScriptWrapper::entityWrap( ServiceProviderInterface * _serviceProvider )
	{
		classWrapping( _serviceProvider );

		pybind::kernel_interface * kernel = pybind::get_kernel();

		pybind::superclass_<Entity, pybind::bases<Node> >( kernel, "Entity", (void *)_serviceProvider, new superclass_new_Entity, nullptr, false )
            .def_constructor( pybind::init<>() )
			.def( "getPrototype", &Entity::getPrototype )
			;

		pybind::superclass_<Arrow, pybind::bases<Entity> >( kernel, "Arrow", (void *)_serviceProvider, new superclass_new_Arrow, nullptr, false )
            .def_constructor( pybind::init<>() )
			.def( "setOffsetClick", &Arrow::setOffsetClick )
			.def( "getOffsetClick", &Arrow::getOffsetClick )
			.def( "setPolygon", &Arrow::setPolygon )
			.def( "getPolygon", &Arrow::getPolygon )
			.def( "setRadius", &Arrow::setRadius )
			.def( "getRadius", &Arrow::getRadius )
			;

		pybind::superclass_<Scene, pybind::bases<Entity> >( kernel, "Scene", (void *)_serviceProvider, new superclass_new_Scene, nullptr, false )
            .def_constructor( pybind::init<>() )
			//.def( "isSubScene", &Scene::isSubScene )
			//.def( "getParentScene", &Scene::getParentScene )
			//.def( "setRenderTarget", &Scene::setRenderTarget )
			//.def( "renderSelf", &Scene::renderSelf )
			//.def( "blockInput", &Scene::blockInput )
			//.def( "getBlockInput", &Scene::getBlockInput )
			//.def( "getMainLayer", &Scene::getMainLayer )
			//.def( "setMainLayer", &Scene::setMainLayer )
			;

        EntityScriptMethod * entityScriptMethod = new EntityScriptMethod(_serviceProvider);

		pybind::def_functor( kernel, "addEntityPrototypeFinder", entityScriptMethod, &EntityScriptMethod::s_addEntityPrototypeFinder );
		pybind::def_functor( kernel, "addScenePrototypeFinder", entityScriptMethod, &EntityScriptMethod::s_addScenePrototypeFinder );
		pybind::def_functor( kernel, "addArrowPrototypeFinder", entityScriptMethod, &EntityScriptMethod::s_addArrowPrototypeFinder );
		pybind::def_functor( kernel, "createEntity", entityScriptMethod, &EntityScriptMethod::s_createEntity );
		pybind::def_functor_kernel( kernel, "importEntity", entityScriptMethod, &EntityScriptMethod::s_importEntity );
		//pybind::def_function( "createEntityFromBinary", &ScriptMethod::createEntityFromBinary );
	}
}
