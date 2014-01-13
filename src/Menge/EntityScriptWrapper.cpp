#	include "ScriptWrapper.h"

#	include "ScriptClassWrapper.h"

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
        class PythonPrototypeGenerator
            : public PrototypeGeneratorInterface
        {
        public:
            PythonPrototypeGenerator( ServiceProviderInterface * _serviceProvider, const ConstString & _category, const ConstString & _prototype, PyObject * _module )
                : m_serviceProvider(_serviceProvider)
                , m_category(_category)
                , m_prototype(_prototype)
                , m_module(_module)
            {
                pybind::incref( m_module );
            }

            virtual ~PythonPrototypeGenerator()
            {
                pybind::decref( m_module );
				m_module = nullptr;
            }

        public:
            PyObject * getModule() const
            {
                return m_module;
            }

        public:
            void destroy() override
            {
                delete this;
            }

        protected:
            ServiceProviderInterface * m_serviceProvider;
            ConstString m_category;
            ConstString m_prototype;
            PyObject * m_module;
        };
		//////////////////////////////////////////////////////////////////////////
		typedef stdex::intrusive_ptr<PythonPrototypeGenerator> PythonPrototypeGeneratorPtr;
        //////////////////////////////////////////////////////////////////////////
        class EntityPrototypeGenerator
            : public PythonPrototypeGenerator
        {
        public:
            EntityPrototypeGenerator( ServiceProviderInterface * _serviceProvider, const ConstString & _category, const ConstString & _prototype, PyObject * _module )
                : PythonPrototypeGenerator(_serviceProvider, _category, _prototype, _module)
            {
            }

        protected:
            Factorable * generate( const ConstString & _category, const ConstString & _prototype ) override
            {
                (void)_category;
                (void)_prototype;

				PyObject * py_type = pybind::ask( m_module, "(O)", pybind::ptr(_prototype) );

				if( py_type == nullptr )
				{
					LOGGER_ERROR(m_serviceProvider)("PythonPrototypeGenerator prototype %s invalid type create"
						, m_category.c_str()
						, m_prototype.c_str()
						);

					return nullptr;
				}

				if( pybind::type_initialize( py_type ) == false )
				{
					LOGGER_ERROR(m_serviceProvider)("PythonPrototypeGenerator prototype %s invalid type initialize"
						, m_category.c_str()
						, m_prototype.c_str()
						);

					return nullptr;
				}

                Entity * entity = SCRIPT_SERVICE(m_serviceProvider)
                    ->createEntityT<Entity>( CONST_STRING(m_serviceProvider, Entity), m_prototype, py_type );

                if( entity == nullptr )
                {
                    LOGGER_ERROR(m_serviceProvider)( "EntityPrototypeGenerator can't generate %s %s"
                        , m_category.c_str()
                        , m_prototype.c_str()
                        );

                    return nullptr;
                }

                return entity;
            }

        protected:
            size_t count() const override
            {
                return 0;
            }
        };
        //////////////////////////////////////////////////////////////////////////
        bool s_addPrototypeFinder( const ConstString & _category, const ConstString & _prototype, PyObject * _module )
        {
            EntityPrototypeGenerator * generator = 
                new EntityPrototypeGenerator(m_serviceProvider, _category, _prototype, _module);

            PROTOTYPE_SERVICE(m_serviceProvider)
                ->addPrototype( _category, _prototype, generator );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_addEntityPrototypeFinder( const ConstString & _prototype, PyObject * _module )
        {
            bool result = s_addPrototypeFinder( CONST_STRING(m_serviceProvider, Entity), _prototype, _module );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_addScenePrototypeFinder( const ConstString & _prototype, PyObject * _module )
        {
            bool result = s_addPrototypeFinder( CONST_STRING(m_serviceProvider, Scene), _prototype, _module );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_addArrowPrototypeFinder( const ConstString & _prototype, PyObject * _module )
        {
            bool result = s_addPrototypeFinder( CONST_STRING(m_serviceProvider, Arrow), _prototype, _module );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
		PyObject * s_createEntity( const ConstString & _prototype )
		{
			Entity * entity = PROTOTYPE_SERVICE(m_serviceProvider)
				->generatePrototypeT<Entity>( CONST_STRING(m_serviceProvider, Entity), _prototype );

			if( entity == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)( "Error: can't create Entity '%s'"
					, _prototype.c_str()
					);

				return nullptr;
			}

			NODE_SERVICE(m_serviceProvider)
				->addHomeless( entity );

			PyObject * py_embedding = entity->getEmbed();

			pybind::incref( py_embedding );

			return py_embedding;
		}
        //////////////////////////////////////////////////////////////////////////
        PyObject * s_importEntity( const ConstString & _prototype )
        {
            PrototypeGeneratorInterfacePtr generator;
            if( PROTOTYPE_SERVICE(m_serviceProvider)
                ->hasPrototype( CONST_STRING(m_serviceProvider, Entity), _prototype, generator ) == false )
            {
                LOGGER_ERROR(m_serviceProvider)( "Error: can't import Entity '%s'"
                    , _prototype.c_str()
                    );

                return pybind::ret_none();
            }

            PythonPrototypeGeneratorPtr pythonGenerator = 
				stdex::intrusive_dynamic_cast<PythonPrototypeGeneratorPtr>(generator);

            if( pythonGenerator == nullptr )
            {
                return pybind::ret_none();
            }

            PyObject * py_module = pythonGenerator->getModule();

            pybind::incref( py_module );

            return py_module;
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
    static void * superclass_new_Entity( const pybind::class_type_scope_ptr & _scope, PyObject * _obj, PyObject * _args, PyObject * _kwds )
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
    static void * superclass_new_Arrow( const pybind::class_type_scope_ptr & _scope, PyObject * _obj, PyObject * _args, PyObject * _kwds )
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
    static void * superclass_new_Scene( const pybind::class_type_scope_ptr & _scope, PyObject * _obj, PyObject * _args, PyObject * _kwds )
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
    static void superclass_dealloc_only_python( const pybind::class_type_scope_ptr & _scope, void * _impl )
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
			.def( "setOffsetClick", &Arrow::setPointClick )
			.def( "getOffsetClick", &Arrow::getPointClick )
			.def( "setPolygon", &Arrow::setPolygon )
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

        pybind::def_functor( "addEntityPrototypeFinder", entityScriptMethod, &EntityScriptMethod::s_addEntityPrototypeFinder );
        pybind::def_functor( "addScenePrototypeFinder", entityScriptMethod, &EntityScriptMethod::s_addScenePrototypeFinder );
        pybind::def_functor( "addArrowPrototypeFinder", entityScriptMethod, &EntityScriptMethod::s_addArrowPrototypeFinder );
		pybind::def_functor( "createEntity", entityScriptMethod, &EntityScriptMethod::s_createEntity );
        pybind::def_functor( "importEntity", entityScriptMethod, &EntityScriptMethod::s_importEntity );
		//pybind::def_function( "createEntityFromBinary", &ScriptMethod::createEntityFromBinary );
	}
}
