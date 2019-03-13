#include "EntityScriptEmbedding.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"

#include "Environment/Python/PythonScriptWrapper.h"

#include "DocumentTraceback.h"
#include "PythonEntityBehavior.h"
#include "EntityPrototypeGenerator.h"

#include "Kernel/Entity.h"
#include "Kernel/Scene.h"
#include "Kernel/Arrow.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    class EntityScriptMethod
        : public Factorable
    {
    public:
        EntityScriptMethod()
        {
            m_factoryEntityPrototypeGenerator = new FactoryPool<EntityPrototypeGenerator, 64>();
        }

        ~EntityScriptMethod() override
        {
            MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryEntityPrototypeGenerator );

            m_factoryEntityPrototypeGenerator = nullptr;
        }

    public:
        //////////////////////////////////////////////////////////////////////////
        bool s_addPrototypeFinder( pybind::kernel_interface * _kernel, const ConstString & _category, const ConstString & _prototype, const pybind::object & _generator )
        {
            EntityPrototypeGeneratorPtr generator = m_factoryEntityPrototypeGenerator->createObject( MENGINE_DOCUMENT_PYBIND );

            generator->setKernel( _kernel );
            generator->setGenerator( _generator );

            bool successful = PROTOTYPE_SERVICE()
                ->addPrototype( _category, _prototype, generator );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_addEntityPrototypeFinder( pybind::kernel_interface * _kernel, const ConstString & _prototype, const pybind::object & _module )
        {
            bool result = s_addPrototypeFinder( _kernel, STRINGIZE_STRING_LOCAL( "Entity" ), _prototype, _module );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_addScenePrototypeFinder( pybind::kernel_interface * _kernel, const ConstString & _prototype, const pybind::object & _module )
        {
            bool result = s_addPrototypeFinder( _kernel, STRINGIZE_STRING_LOCAL( "Scene" ), _prototype, _module );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_addArrowPrototypeFinder( pybind::kernel_interface * _kernel, const ConstString & _prototype, const pybind::object & _module )
        {
            bool result = s_addPrototypeFinder( _kernel, STRINGIZE_STRING_LOCAL( "Arrow" ), _prototype, _module );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        const pybind::object & s_createEntity( pybind::kernel_interface * _kernel, const ConstString & _prototype )
        {
            Char pytraceback[4096];
            _kernel->get_traceback( pytraceback, 4096 );

            EntityPtr entity = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Entity" ), _prototype, pytraceback );

            if( entity == nullptr )
            {
                LOGGER_ERROR( "can't create entity '%s'"
                    , _prototype.c_str()
                );

                return pybind::object::get_invalid();
            }

            const PythonEntityBehaviorPtr & behavior = entity->getBehavior();
            const pybind::object & py_entity = behavior->getScriptObject();

            return py_entity;
        }
        //////////////////////////////////////////////////////////////////////////
        pybind::object s_importEntity( pybind::kernel_interface * _kernel, const ConstString & _prototype )
        {
            (void)_kernel;

            const PrototypeGeneratorInterfacePtr & generator = PROTOTYPE_SERVICE()
                ->getGenerator( STRINGIZE_STRING_LOCAL( "Entity" ), _prototype );

            if( generator == nullptr )
            {
                LOGGER_ERROR( "importEntity: can't import 'Entity' '%s'"
                    , _prototype.c_str()
                );

                return pybind::make_none_t( _kernel );
            }

            EntityPrototypeGeneratorPtr entityGenerator =
                stdex::intrusive_static_cast<EntityPrototypeGeneratorPtr>(generator);

            const pybind::object & py_type = entityGenerator->getPythonType();

            return py_type;
        }

    protected:
        FactoryPtr m_factoryEntityPrototypeGenerator;
    };
    //////////////////////////////////////////////////////////////////////////
    class superclass_new_Entity
        : public pybind::new_adapter_interface
    {
    public:
        void * call( pybind::kernel_interface * _kernel, const pybind::class_type_scope_interface_ptr & _scope, PyObject * _obj, PyObject * _args, PyObject * _kwds ) override
        {
            (void)_scope;
            (void)_args;
            (void)_kwds;

            Char pytraceback[4096];
            _kernel->get_traceback( pytraceback, 4096 );

            EntityPtr entity = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Entity" ), pytraceback );

            entity->setEmbed( _kernel, _obj );

            _kernel->decref( _obj );

            Entity * entity_ptr = entity.get();

            IntrusivePtrBase::intrusive_ptr_add_ref( entity_ptr );

            return entity_ptr;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    class superclass_destroy_Entity
        : public pybind::destroy_adapter_interface
    {
    public:
        void call( pybind::kernel_interface * _kernel, const pybind::class_type_scope_interface_ptr & _scope, void * _impl ) override
        {
            (void)_kernel;
            (void)_scope;

            Entity * entity_ptr = static_cast<Entity *>(_impl);

            IntrusivePtrBase::intrusive_ptr_dec_ref( entity_ptr );
        }
    };
    //////////////////////////////////////////////////////////////////////////
    class superclass_new_Arrow
        : public pybind::new_adapter_interface
    {
    public:
        void * call( pybind::kernel_interface * _kernel, const pybind::class_type_scope_interface_ptr & _scope, PyObject * _obj, PyObject * _args, PyObject * _kwds ) override
        {
            (void)_scope;
            (void)_args;
            (void)_kwds;

            Char pytraceback[4096];
            _kernel->get_traceback( pytraceback, 4096 );

            ArrowPtr arrow = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Arrow" ), pytraceback );

            arrow->setEmbed( _kernel, _obj );

            _kernel->decref( _obj );

            Arrow * arrow_ptr = arrow.get();

            IntrusivePtrBase::intrusive_ptr_add_ref( arrow_ptr );

            return arrow_ptr;
        }
    };
    //////////////////////////////////////////////////////////////////////////    
    class superclass_destroy_Arrow
        : public pybind::destroy_adapter_interface
    {
    public:
        void call( pybind::kernel_interface * _kernel, const pybind::class_type_scope_interface_ptr & _scope, void * _impl ) override
        {
            (void)_kernel;
            (void)_scope;

            Arrow * arrow_ptr = static_cast<Arrow *>(_impl);

            IntrusivePtrBase::intrusive_ptr_dec_ref( arrow_ptr );
        }
    };
    //////////////////////////////////////////////////////////////////////////
    class superclass_new_Scene
        : public pybind::new_adapter_interface
    {
    public:
        void * call( pybind::kernel_interface * _kernel, const pybind::class_type_scope_interface_ptr & _scope, PyObject * _obj, PyObject * _args, PyObject * _kwds ) override
        {
            (void)_scope;
            (void)_args;
            (void)_kwds;

            Char pytraceback[4096];
            _kernel->get_traceback( pytraceback, 4096 );

            ScenePtr scene = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Scene" ), pytraceback );

            scene->setEmbed( _kernel, _obj );

            _kernel->decref( _obj );

            Scene * scene_ptr = scene.get();

            IntrusivePtrBase::intrusive_ptr_add_ref( scene_ptr );

            return scene_ptr;
        }
    };
    //////////////////////////////////////////////////////////////////////////    
    class superclass_destroy_Scene
        : public pybind::destroy_adapter_interface
    {
    public:
        void call( pybind::kernel_interface * _kernel, const pybind::class_type_scope_interface_ptr & _scope, void * _impl ) override
        {
            (void)_kernel;
            (void)_scope;

            Scene * scene_ptr = static_cast<Scene *>(_impl);

            IntrusivePtrBase::intrusive_ptr_dec_ref( scene_ptr );
        }
    };
    //////////////////////////////////////////////////////////////////////////
    EntityScriptEmbedding::EntityScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EntityScriptEmbedding::~EntityScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool EntityScriptEmbedding::embedding( pybind::kernel_interface * _kernel )
    {
        pybind::superclass_<Entity, pybind::bases<Node> >( _kernel, "Entity", nullptr, pybind::new_adapter_interface_ptr( new superclass_new_Entity ), pybind::destroy_adapter_interface_ptr( new superclass_destroy_Entity ), false )
            .def_constructor( pybind::init<>() )
            .def( "getPrototype", &Entity::getPrototype )
            ;

        pybind::superclass_<Arrow, pybind::bases<Entity> >( _kernel, "Arrow", nullptr, pybind::new_adapter_interface_ptr( new superclass_new_Arrow ), pybind::destroy_adapter_interface_ptr( new superclass_destroy_Arrow ), false )
            .def_constructor( pybind::init<>() )
            .def( "setOffsetClick", &Arrow::setOffsetClick )
            .def( "getOffsetClick", &Arrow::getOffsetClick )
            .def( "setPolygon", &Arrow::setPolygon )
            .def( "getPolygon", &Arrow::getPolygon )
            .def( "setRadius", &Arrow::setRadius )
            .def( "getRadius", &Arrow::getRadius )
            ;

        pybind::superclass_<Scene, pybind::bases<Entity> >( _kernel, "Scene", nullptr, pybind::new_adapter_interface_ptr( new superclass_new_Scene ), pybind::destroy_adapter_interface_ptr( new superclass_destroy_Scene ), false )
            .def_constructor( pybind::init<>() )
            ;

        EntityScriptMethod * entityScriptMethod = new FactorableUnique<EntityScriptMethod>();

        pybind::def_functor_kernel( _kernel, "addEntityPrototypeFinder", entityScriptMethod, &EntityScriptMethod::s_addEntityPrototypeFinder );
        pybind::def_functor_kernel( _kernel, "addScenePrototypeFinder", entityScriptMethod, &EntityScriptMethod::s_addScenePrototypeFinder );
        pybind::def_functor_kernel( _kernel, "addArrowPrototypeFinder", entityScriptMethod, &EntityScriptMethod::s_addArrowPrototypeFinder );
        pybind::def_functor_kernel( _kernel, "createEntity", entityScriptMethod, &EntityScriptMethod::s_createEntity );
        pybind::def_functor_kernel( _kernel, "importEntity", entityScriptMethod, &EntityScriptMethod::s_importEntity );

        m_implement = entityScriptMethod;

        VOCABULARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "Arrow" ), new FactorableUnique<PythonScriptWrapper<Arrow> >( _kernel ) );
        VOCABULARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "Entity" ), new FactorableUnique<PythonScriptWrapper<Entity> >( _kernel ) );
        VOCABULARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "Scene" ), new FactorableUnique<PythonScriptWrapper<Scene> >( _kernel ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void EntityScriptEmbedding::ejecting()
    {
        m_implement = nullptr;

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "Arrow" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "Entity" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "Scene" ) );
    }
}
