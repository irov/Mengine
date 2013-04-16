#	include "ScriptWrapper.h"

#	include "ScriptClassWrapper.h"

#	include "Kernel/Entity.h"
#	include "Kernel/Scene.h"

#	include "Arrow.h"

#   include "Interface/ResourceInterface.h"
#   include "Interface/ScriptSystemInterface.h"
#   include "Interface/EntityInterface.h"
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
		PyObject * s_createEntity( const ConstString & _prototype )
		{
			Entity * entity = ENTITY_SERVICE(m_serviceProvider)
				->createEntity( _prototype );

			if( entity == 0 )
			{
				LOGGER_ERROR(m_serviceProvider)( "Error: can't create Entity '%s'"
					, _prototype.c_str()
					);

				return 0;
			}

			NODE_SERVICE(m_serviceProvider)
				->addHomeless( entity );

			PyObject * py_embedding = entity->getEmbed();

			pybind::incref( py_embedding );

			return py_embedding;
		}

        PyObject * s_importEntity( const ConstString & _type )
        {
            PyObject * py_prototype = ENTITY_SERVICE(m_serviceProvider)
                ->getPrototype( _type );

            if( py_prototype == 0 )
            {
                return pybind::ret_none();
            }

            pybind::incref( py_prototype );

            return py_prototype;
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
    static void * superclass_new_Entity( pybind::class_type_scope * _scope, PyObject * _obj, PyObject * _args, PyObject * _kwds )
    {
        (void)_obj;
        (void)_args;
        (void)_kwds;

        void * user = _scope->get_user();
        ServiceProviderInterface * serviceProvider = static_cast<ServiceProviderInterface *>(user);

        Entity * entity = NODE_SERVICE(serviceProvider)
            ->createNodeT<Entity>( CONST_STRING(serviceProvider, Entity) );

        return entity;
    }
    //////////////////////////////////////////////////////////////////////////
    static void * superclass_new_Arrow( pybind::class_type_scope * _scope, PyObject * _obj, PyObject * _args, PyObject * _kwds )
    {
        (void)_obj;
        (void)_args;
        (void)_kwds;

        void * user = _scope->get_user();
        ServiceProviderInterface * serviceProvider = static_cast<ServiceProviderInterface *>(user);

        Arrow * arrow = NODE_SERVICE(serviceProvider)
            ->createNodeT<Arrow>( CONST_STRING(serviceProvider, Arrow) );

        return arrow;
    }
    //////////////////////////////////////////////////////////////////////////
    static void * superclass_new_Scene( pybind::class_type_scope * _scope, PyObject * _obj, PyObject * _args, PyObject * _kwds )
    {
        (void)_obj;
        (void)_args;
        (void)_kwds;

        void * user = _scope->get_user();
        ServiceProviderInterface * serviceProvider = static_cast<ServiceProviderInterface *>(user);

        Scene * scene = NODE_SERVICE(serviceProvider)
            ->createNodeT<Scene>( CONST_STRING(serviceProvider, Scene) );

        return scene;
    }
    //////////////////////////////////////////////////////////////////////////
    static void superclass_dealloc_only_python( pybind::class_type_scope * _scope, void * _impl )
    {
        (void)_scope;
        (void)_impl;
        //Empty
    }
	//////////////////////////////////////////////////////////////////////////
	void ScriptWrapper::entityWrap( ServiceProviderInterface * _serviceProvider )
	{
		classWrapping( _serviceProvider );

		pybind::superclass_<Entity, pybind::bases<Node> >("Entity", (void *)_serviceProvider, &superclass_new_Entity, &superclass_dealloc_only_python, false)
            .def_constructor( pybind::init<>() )
			.def( "getPrototype", &Entity::getPrototype )
			;

		pybind::superclass_<Arrow, pybind::bases<Entity> >("Arrow", (void *)_serviceProvider, &superclass_new_Arrow, &superclass_dealloc_only_python, false)
            .def_constructor( pybind::init<>() )
			.def( "setOffsetClick", &Arrow::setOffsetClick )
			.def( "getOffsetClick", &Arrow::getOffsetClick )
			.def( "setPolygon", &Arrow::setPolygon )
			.def( "removePolygon", &Arrow::removePolygon )
			.def( "getPolygon", &Arrow::getPolygon )
			.def( "setRadius", &Arrow::setRadius )
			.def( "getRadius", &Arrow::getRadius )
			;

		pybind::superclass_<Scene, pybind::bases<Entity> >("Scene", (void *)_serviceProvider, &superclass_new_Scene, &superclass_dealloc_only_python, false)
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

		pybind::def_functor( "createEntity", entityScriptMethod, &EntityScriptMethod::s_createEntity );
        pybind::def_functor( "importEntity", entityScriptMethod, &EntityScriptMethod::s_importEntity );
		//pybind::def_function( "createEntityFromBinary", &ScriptMethod::createEntityFromBinary );
	}
}
