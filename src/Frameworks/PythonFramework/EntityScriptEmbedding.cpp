#include "EntityScriptEmbedding.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"

#include "Environment/Python/PythonScriptWrapper.h"

#include "DocumentTraceback.h"
#include "PythonEntityBehavior.h"
#include "PythonEntityPrototypeGenerator.h"

#include "Kernel/Entity.h"
#include "Kernel/Scene.h"
#include "Kernel/Arrow.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/ConstStringHelper.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    class EntityScriptMethod
        : public Factorable
    {
    public:
        EntityScriptMethod()
        {
            m_factoryEntityPrototypeGenerator = new FactoryPool<PythonEntityPrototypeGenerator, 64>();
        }

        ~EntityScriptMethod() override
        {
            for( const PythonEntityPrototypeGeneratorPtr & generator : m_entityPrototypeGenerators )
            {
                const ConstString & category = generator->getCategory();
                const ConstString & prototype = generator->getPrototype();

                PROTOTYPE_SERVICE()
                    ->removePrototype( category, prototype );
            }

            m_entityPrototypeGenerators.clear();

            MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryEntityPrototypeGenerator );

            m_factoryEntityPrototypeGenerator = nullptr;
        }

    public:
        //////////////////////////////////////////////////////////////////////////
        bool s_addPrototypeFinder( const ConstString & _category, const ConstString & _prototype, const pybind::object & _generator )
        {
            PythonEntityPrototypeGeneratorPtr generator = m_factoryEntityPrototypeGenerator->createObject( MENGINE_DOCUMENT_PYBIND );

            generator->setGenerator( _generator );

            m_entityPrototypeGenerators.emplace_back( generator );

            bool successful = PROTOTYPE_SERVICE()
                ->addPrototype( _category, _prototype, generator );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_addEntityPrototypeFinder( const ConstString & _prototype, const pybind::object & _module )
        {
            bool result = s_addPrototypeFinder( STRINGIZE_STRING_LOCAL( "Entity" ), _prototype, _module );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_addScenePrototypeFinder( const ConstString & _prototype, const pybind::object & _module )
        {
            bool result = s_addPrototypeFinder( STRINGIZE_STRING_LOCAL( "Scene" ), _prototype, _module );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_addArrowPrototypeFinder( const ConstString & _prototype, const pybind::object & _module )
        {
            bool result = s_addPrototypeFinder( STRINGIZE_STRING_LOCAL( "Arrow" ), _prototype, _module );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        const pybind::object & s_createEntity( const ConstString & _prototype )
        {
            EntityPtr entity = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Entity" ), _prototype, MENGINE_DOCUMENT_PYBIND );

            MENGINE_ASSERTION_MEMORY_PANIC( entity, pybind::object::get_invalid(), "can't create entity '%s'"
                , _prototype.c_str()
            );

            const PythonEntityBehaviorPtr & behavior = entity->getBehavior();
            const pybind::object & py_entity = behavior->getScriptObject();

            return py_entity;
        }
        //////////////////////////////////////////////////////////////////////////
        pybind::object s_importEntity( pybind::kernel_interface * _kernel, const ConstString & _prototype )
        {
            MENGINE_UNUSED( _kernel );

            const PrototypeGeneratorInterfacePtr & generator = PROTOTYPE_SERVICE()
                ->getGenerator( STRINGIZE_STRING_LOCAL( "Entity" ), _prototype );

            MENGINE_ASSERTION_MEMORY_PANIC( generator, pybind::make_none_t( _kernel ), "importEntity: can't import 'Entity' '%s'"
                , _prototype.c_str()
            );

            PythonEntityPrototypeGeneratorPtr entityGenerator =
                stdex::intrusive_static_cast<PythonEntityPrototypeGeneratorPtr>(generator);

            const pybind::object & py_type = entityGenerator->getPythonType();

            return py_type;
        }

    protected:
        FactoryPtr m_factoryEntityPrototypeGenerator;

        typedef Vector<PythonEntityPrototypeGeneratorPtr> VectorEntityPrototypeGenerators;
        VectorEntityPrototypeGenerators m_entityPrototypeGenerators;
    };
    //////////////////////////////////////////////////////////////////////////
    class superclass_new_Entity
        : public pybind::new_adapter_interface
    {
    public:
        void * call( pybind::kernel_interface * _kernel, const pybind::class_type_scope_interface_ptr & _scope, PyObject * _obj, PyObject * _args, PyObject * _kwds ) override
        {
            MENGINE_UNUSED( _scope );
            MENGINE_UNUSED( _args );
            MENGINE_UNUSED( _kwds );

            EntityPtr entity = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Entity" ), MENGINE_DOCUMENT_PYBIND );

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
            MENGINE_UNUSED( _kernel );
            MENGINE_UNUSED( _scope );

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
            MENGINE_UNUSED( _scope );
            MENGINE_UNUSED( _args );
            MENGINE_UNUSED( _kwds );

            ArrowPtr arrow = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Arrow" ), MENGINE_DOCUMENT_PYBIND );

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
            MENGINE_UNUSED( _kernel );
            MENGINE_UNUSED( _scope );

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
            MENGINE_UNUSED( _scope );
            MENGINE_UNUSED( _args );
            MENGINE_UNUSED( _kwds );

            ScenePtr scene = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Scene" ), MENGINE_DOCUMENT_PYBIND );

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
            MENGINE_UNUSED( _kernel );
            MENGINE_UNUSED( _scope );

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

        pybind::def_functor( _kernel, "addEntityPrototypeFinder", entityScriptMethod, &EntityScriptMethod::s_addEntityPrototypeFinder );
        pybind::def_functor( _kernel, "addScenePrototypeFinder", entityScriptMethod, &EntityScriptMethod::s_addScenePrototypeFinder );
        pybind::def_functor( _kernel, "addArrowPrototypeFinder", entityScriptMethod, &EntityScriptMethod::s_addArrowPrototypeFinder );
        pybind::def_functor( _kernel, "createEntity", entityScriptMethod, &EntityScriptMethod::s_createEntity );
        pybind::def_functor_kernel( _kernel, "importEntity", entityScriptMethod, &EntityScriptMethod::s_importEntity );

        m_implement = entityScriptMethod;

        VOCABULARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "Arrow" ), new FactorableUnique<PythonScriptWrapper<Arrow> >( _kernel ) );
        VOCABULARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "Entity" ), new FactorableUnique<PythonScriptWrapper<Entity> >( _kernel ) );
        VOCABULARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "Scene" ), new FactorableUnique<PythonScriptWrapper<Scene> >( _kernel ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void EntityScriptEmbedding::ejecting( pybind::kernel_interface * _kernel )
    {
        MENGINE_UNUSED( _kernel );

        m_implement = nullptr;

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "Arrow" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "Entity" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "Scene" ) );
    }
}
