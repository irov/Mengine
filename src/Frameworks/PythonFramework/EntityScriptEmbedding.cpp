#include "EntityScriptEmbedding.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonScriptWrapper.h"
#include "Environment/Python/PythonDocumentTraceback.h"

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
#include "Kernel/FactorableUnique.h"

#include "Config/Algorithm.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class EntityScriptMethod
        : public Factorable
    {
    public:
        EntityScriptMethod()
        {
        }

        ~EntityScriptMethod() override
        {
        }

    public:
        bool initialize()
        {
            m_factoryEntityPrototypeGenerator = Helper::makeFactoryPool<PythonEntityPrototypeGenerator, 64>( MENGINE_DOCUMENT_FACTORABLE );

            return true;
        }

        void finalize()
        {
            for( const PrototypeGeneratorInterfacePtr & generator : m_entityPrototypeGenerators )
            {
                const ConstString & category = generator->getCategory();
                const ConstString & prototype = generator->getPrototype();

                PROTOTYPE_SERVICE()
                    ->removePrototype( category, prototype, nullptr );
            }

            m_entityPrototypeGenerators.clear();

            MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryEntityPrototypeGenerator );

            m_factoryEntityPrototypeGenerator = nullptr;
        }

    public:
        //////////////////////////////////////////////////////////////////////////
        bool s_hasPrototypeFinder( const ConstString & _category, const ConstString & _prototype )
        {
            bool result = PROTOTYPE_SERVICE()
                ->hasPrototype( _category, _prototype, nullptr );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_hasEntityPrototypeFinder( const ConstString & _prototype )
        {
            bool result = s_hasPrototypeFinder( STRINGIZE_STRING_LOCAL( "Entity" ), _prototype );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_hasScenePrototypeFinder( const ConstString & _prototype )
        {
            bool result = s_hasPrototypeFinder( STRINGIZE_STRING_LOCAL( "Scene" ), _prototype );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_hasArrowPrototypeFinder( const ConstString & _prototype )
        {
            bool result = s_hasPrototypeFinder( STRINGIZE_STRING_LOCAL( "Arrow" ), _prototype );

            return result;
        }
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
        void s_removePrototypeFinder( const ConstString & _category, const ConstString & _prototype )
        {
            PrototypeGeneratorInterfacePtr generator;
            PROTOTYPE_SERVICE()
                ->removePrototype( _category, _prototype, &generator );

            MENGINE_ASSERTION_FATAL( generator != nullptr, "removing generator category '%s' prototype '%s' not found"
                , _category.c_str()
                , _prototype.c_str()
            );

            VectorEntityPrototypeGenerators::iterator it_found = Algorithm::find( m_entityPrototypeGenerators.begin(), m_entityPrototypeGenerators.end(), generator );

            MENGINE_ASSERTION_FATAL( it_found != m_entityPrototypeGenerators.end(), "remove generator category '%s' prototype '%s' not add from script"
                , _category.c_str()
                , _prototype.c_str()
            );

            *it_found = m_entityPrototypeGenerators.back();
            m_entityPrototypeGenerators.pop_back();
        }
        //////////////////////////////////////////////////////////////////////////
        void s_removeEntityPrototypeFinder( const ConstString & _prototype )
        {
            s_removePrototypeFinder( STRINGIZE_STRING_LOCAL( "Entity" ), _prototype );
        }
        //////////////////////////////////////////////////////////////////////////
        void s_removeScenePrototypeFinder( const ConstString & _prototype )
        {
            s_removePrototypeFinder( STRINGIZE_STRING_LOCAL( "Scene" ), _prototype );
        }
        //////////////////////////////////////////////////////////////////////////
        void s_removeArrowPrototypeFinder( const ConstString & _prototype )
        {
            s_removePrototypeFinder( STRINGIZE_STRING_LOCAL( "Arrow" ), _prototype );
        }
        //////////////////////////////////////////////////////////////////////////
        pybind::object s_createEntity( const ConstString & _prototype )
        {
            EntityPtr entity = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Entity" ), _prototype, MENGINE_DOCUMENT_PYBIND );

            MENGINE_ASSERTION_MEMORY_PANIC( entity, "can't create entity '%s'"
                , _prototype.c_str()
            );

            const PythonEntityBehaviorPtr & behavior = entity->getBehavior();
            const pybind::object & py_entity = behavior->getScriptObject();

            return py_entity;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_destroyEntity( const EntityPtr & _entity )
        {
            _entity->dispose();
        }
        //////////////////////////////////////////////////////////////////////////
        pybind::object s_importEntity( pybind::kernel_interface * _kernel, const ConstString & _prototype )
        {
            MENGINE_UNUSED( _kernel );

            const PrototypeGeneratorInterfacePtr & generator = PROTOTYPE_SERVICE()
                ->getGenerator( STRINGIZE_STRING_LOCAL( "Entity" ), _prototype );

            MENGINE_ASSERTION_MEMORY_PANIC( generator, "importEntity: can't import 'Entity' '%s'"
                , _prototype.c_str()
            );

            PythonEntityPrototypeGeneratorPtr entityGenerator =
                stdex::intrusive_static_cast<PythonEntityPrototypeGeneratorPtr>(generator);

            const pybind::object & py_type = entityGenerator->getPythonType();

            return py_type;
        }

    protected:
        FactoryInterfacePtr m_factoryEntityPrototypeGenerator;

        typedef Vector<PrototypeGeneratorInterfacePtr> VectorEntityPrototypeGenerators;
        VectorEntityPrototypeGenerators m_entityPrototypeGenerators;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<EntityScriptMethod> EntityScriptMethodPtr;
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
    bool EntityScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::allocator_interface * allocator = _kernel->get_allocator();

        pybind::superclass_<Entity, pybind::bases<Node>>( _kernel, "Entity", nullptr, allocator->newT<superclass_new_Entity>(), allocator->newT<superclass_destroy_Entity>(), false )
            .def_constructor( pybind::init<>() )
            .def( "getPrototype", &Entity::getPrototype )
            ;

        pybind::superclass_<Arrow, pybind::bases<Entity>>( _kernel, "Arrow", nullptr, allocator->newT<superclass_new_Arrow>(), allocator->newT<superclass_destroy_Arrow>(), false )
            .def_constructor( pybind::init<>() )
            .def( "setOffsetClick", &Arrow::setOffsetClick )
            .def( "getOffsetClick", &Arrow::getOffsetClick )
            .def( "setPolygon", &Arrow::setPolygon )
            .def( "getPolygon", &Arrow::getPolygon )
            .def( "setRadius", &Arrow::setRadius )
            .def( "getRadius", &Arrow::getRadius )
            ;

        pybind::superclass_<Scene, pybind::bases<Entity>>( _kernel, "Scene", nullptr, allocator->newT<superclass_new_Scene>(), allocator->newT<superclass_destroy_Scene>(), false )
            .def_constructor( pybind::init<>() )
            ;

        EntityScriptMethodPtr entityScriptMethod = Helper::makeFactorableUnique<EntityScriptMethod>( MENGINE_DOCUMENT_FACTORABLE );

        if( entityScriptMethod->initialize() == false )
        {
            return false;
        }

        pybind::def_functor( _kernel, "hasPrototypeFinder", entityScriptMethod, &EntityScriptMethod::s_hasPrototypeFinder );
        pybind::def_functor( _kernel, "hasEntityPrototypeFinder", entityScriptMethod, &EntityScriptMethod::s_hasEntityPrototypeFinder );
        pybind::def_functor( _kernel, "hasScenePrototypeFinder", entityScriptMethod, &EntityScriptMethod::s_hasScenePrototypeFinder );
        pybind::def_functor( _kernel, "hasArrowPrototypeFinder", entityScriptMethod, &EntityScriptMethod::s_hasArrowPrototypeFinder );
        
        pybind::def_functor( _kernel, "removePrototypeFinder", entityScriptMethod, &EntityScriptMethod::s_removePrototypeFinder );
        pybind::def_functor( _kernel, "removeEntityPrototypeFinder", entityScriptMethod, &EntityScriptMethod::s_removeEntityPrototypeFinder );
        pybind::def_functor( _kernel, "removeScenePrototypeFinder", entityScriptMethod, &EntityScriptMethod::s_removeScenePrototypeFinder );
        pybind::def_functor( _kernel, "removeArrowPrototypeFinder", entityScriptMethod, &EntityScriptMethod::s_removeArrowPrototypeFinder );

        pybind::def_functor( _kernel, "addPrototypeFinder", entityScriptMethod, &EntityScriptMethod::s_addPrototypeFinder );
        pybind::def_functor( _kernel, "addEntityPrototypeFinder", entityScriptMethod, &EntityScriptMethod::s_addEntityPrototypeFinder );
        pybind::def_functor( _kernel, "addScenePrototypeFinder", entityScriptMethod, &EntityScriptMethod::s_addScenePrototypeFinder );
        pybind::def_functor( _kernel, "addArrowPrototypeFinder", entityScriptMethod, &EntityScriptMethod::s_addArrowPrototypeFinder );

        pybind::def_functor( _kernel, "createEntity", entityScriptMethod, &EntityScriptMethod::s_createEntity );
        pybind::def_functor( _kernel, "destroyEntity", entityScriptMethod, &EntityScriptMethod::s_destroyEntity );
        pybind::def_functor_kernel( _kernel, "importEntity", entityScriptMethod, &EntityScriptMethod::s_importEntity );

        m_implement = entityScriptMethod;

        Helper::registerScriptWrapping<Arrow>( _kernel, STRINGIZE_STRING_LOCAL( "Arrow" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<Entity>( _kernel, STRINGIZE_STRING_LOCAL( "Entity" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<Scene>( _kernel, STRINGIZE_STRING_LOCAL( "Scene" ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void EntityScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        MENGINE_UNUSED( _kernel );

        EntityScriptMethodPtr entityScriptMethod = stdex::intrusive_static_cast<EntityScriptMethodPtr>(m_implement);
        entityScriptMethod->finalize();

        m_implement = nullptr;

        Helper::unregisterScriptWrapping( STRINGIZE_STRING_LOCAL( "Arrow" ) );
        Helper::unregisterScriptWrapping( STRINGIZE_STRING_LOCAL( "Entity" ) );
        Helper::unregisterScriptWrapping( STRINGIZE_STRING_LOCAL( "Scene" ) );
    }
    //////////////////////////////////////////////////////////////////////////
}
