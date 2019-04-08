#include "EntityPrototypeGenerator.h"
#include "PythonEntityBehavior.h"

#include "Environment/Python/PythonEventReceiver.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace
    {
        //////////////////////////////////////////////////////////////////////////
        namespace Detail
        {
            template<class ... Args>
            void callBehavior( const pybind::object & _cb, const EntityBehaviorInterfacePtr & _behavior, Args ... _args )
            {
                PythonEntityBehavior * py_behavior = _behavior.getT<PythonEntityBehavior *>();

                py_behavior->call( _cb, _args ... );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonEntityEventReceiver
            : public PythonEventReceiver
            , public EntityEventReceiver
            , public Factorable
        {
        public:
            void onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior ) override
            {
                Detail::callBehavior( m_cb, _behavior );
            }

            void onEntityActivate( const EntityBehaviorInterfacePtr & _behavior ) override
            {
                Detail::callBehavior( m_cb, _behavior );
            }

            void onEntityPreparationDeactivate( const EntityBehaviorInterfacePtr & _behavior ) override
            {
                Detail::callBehavior( m_cb, _behavior );
            }

            void onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior ) override
            {
                Detail::callBehavior( m_cb, _behavior );
            }

            void onEntityCompile( const EntityBehaviorInterfacePtr & _behavior ) override
            {
                Detail::callBehavior( m_cb, _behavior );
            }

            void onEntityRelease( const EntityBehaviorInterfacePtr & _behavior ) override
            {
                Detail::callBehavior( m_cb, _behavior );
            }

            void onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Node * _node ) override
            {
                Detail::callBehavior( m_cb, _behavior, _node );
            }

            void onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior ) override
            {
                Detail::callBehavior( m_cb, _behavior );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        class PythonEntityEventation
            : public Factorable
            , public Eventable
            , public BaseEventation
        {
            DECLARE_EVENTABLE( EntityEventReceiver );
        };
    }
    //////////////////////////////////////////////////////////////////////////
    EntityPrototypeGenerator::EntityPrototypeGenerator()
        : m_kernel( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EntityPrototypeGenerator::~EntityPrototypeGenerator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void EntityPrototypeGenerator::setKernel( pybind::kernel_interface * _kernel )
    {
        m_kernel = _kernel;
    }
    //////////////////////////////////////////////////////////////////////////
    pybind::kernel_interface * EntityPrototypeGenerator::getKernel() const
    {
        return m_kernel;
    }
    //////////////////////////////////////////////////////////////////////////
    void EntityPrototypeGenerator::setGenerator( const pybind::object & _generator )
    {
        m_generator = _generator;
    }
    //////////////////////////////////////////////////////////////////////////
    const pybind::object & EntityPrototypeGenerator::getGenerator() const
    {
        return m_generator;
    }
    //////////////////////////////////////////////////////////////////////////
    bool EntityPrototypeGenerator::initialize()
    {
        if( FactoryPrototypeGenerator::initialize() == false )
        {
            return false;
        }

        if( m_generator.is_callable() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void EntityPrototypeGenerator::finalize()
    {
        FactoryPrototypeGenerator::finalize();

        m_eventable = nullptr;
        m_generator = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    FactoryPtr EntityPrototypeGenerator::_initializeFactory()
    {
        FactoryPtr factory = Helper::makeFactoryPool<PythonEntityBehavior, 128>();

        return factory;
    }
    //////////////////////////////////////////////////////////////////////////
    const pybind::object & EntityPrototypeGenerator::getPythonType()
    {
        if( m_type.is_invalid() == false )
        {
            return m_type;
        }

        pybind::object py_type = m_generator.call( m_prototype );

        if( py_type.is_invalid() == true || py_type.is_none() == true )
        {
            LOGGER_ERROR( "type '%s' prototype '%s' invalid type create"
                , m_category.c_str()
                , m_prototype.c_str()
            );

            return pybind::object::get_invalid();
        }

        if( py_type.is_type_class() == true )
        {
            PyObject * py_type_ptr = py_type.ptr();
            pybind::kernel_interface * py_kernel = py_type.kernel();

            if( py_kernel->type_initialize( py_type_ptr ) == false )
            {
                LOGGER_ERROR( "type '%s' prototype '%s' invalid type initialize"
                    , m_category.c_str()
                    , m_prototype.c_str()
                );

                return pybind::object::get_invalid();
            }
        }

        m_eventable = Helper::makeFactorableUnique<PythonEntityEventation>();

        Helper::registerPythonEventReceiverMethod<PythonEntityEventReceiver>( m_kernel, py_type, m_eventable, "onCreate", EVENT_ENTITY_CREATE );
        Helper::registerPythonEventReceiverMethod<PythonEntityEventReceiver>( m_kernel, py_type, m_eventable, "onDestroy", EVENT_ENTITY_DESTROY );
        Helper::registerPythonEventReceiverMethod<PythonEntityEventReceiver>( m_kernel, py_type, m_eventable, "onPreparation", EVENT_ENTITY_PREPARATION );
        Helper::registerPythonEventReceiverMethod<PythonEntityEventReceiver>( m_kernel, py_type, m_eventable, "onActivate", EVENT_ENTITY_ACTIVATE );
        Helper::registerPythonEventReceiverMethod<PythonEntityEventReceiver>( m_kernel, py_type, m_eventable, "onPreparationDeactivate", EVENT_ENTITY_PREPARATION_DEACTIVATE );
        Helper::registerPythonEventReceiverMethod<PythonEntityEventReceiver>( m_kernel, py_type, m_eventable, "onDeactivate", EVENT_ENTITY_DEACTIVATE );
        Helper::registerPythonEventReceiverMethod<PythonEntityEventReceiver>( m_kernel, py_type, m_eventable, "onCompile", EVENT_ENTITY_COMPILE );
        Helper::registerPythonEventReceiverMethod<PythonEntityEventReceiver>( m_kernel, py_type, m_eventable, "onRelease", EVENT_ENTITY_RELEASE );

        //this->registerEventMethod( EVENT_ENTITY_DESTROY, "onDestroy", py_type );

        //this->registerEventMethod( EVENT_ENTITY_PREPARATION, "onPreparation", py_type );
        //this->registerEventMethod( EVENT_ENTITY_ACTIVATE, "onActivate", py_type );
        //this->registerEventMethod( EVENT_ENTITY_PREPARATION_DEACTIVATE, "onPreparationDeactivate", py_type );
        //this->registerEventMethod( EVENT_ENTITY_DEACTIVATE, "onDeactivate", py_type );
        //this->registerEventMethod( EVENT_ENTITY_COMPILE, "onCompile", py_type );
        //this->registerEventMethod( EVENT_ENTITY_RELEASE, "onRelease", py_type );

        m_type = py_type;

        return m_type;
    }
    //////////////////////////////////////////////////////////////////////////
    FactorablePointer EntityPrototypeGenerator::generate( const Char * _doc )
    {
        const pybind::object & py_type = this->getPythonType();

        if( py_type.is_invalid() == true )
        {
            return nullptr;
        }

        pybind::object py_entity = py_type.call();

        if( py_entity.is_invalid() == true )
        {
            LOGGER_ERROR( "can't create object '%s' '%s' (invalid create) doc '%s'"
                , m_category.c_str()
                , m_prototype.c_str()
                , _doc
            );

            return nullptr;
        }

        EntityPtr entity = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), m_category, _doc );

        if( entity == nullptr )
        {
            LOGGER_ERROR( "can't generate '%s' '%s' doc '%s'"
                , m_category.c_str()
                , m_prototype.c_str()
                , _doc
            );

            return nullptr;
        }

        entity->setPrototype( m_prototype );

        entity->setBehaviorEventable( m_eventable );

        const FactoryPtr & factory = this->getFactory();

        PythonEntityBehaviorPtr behavior = factory->createObject( _doc );
        behavior->setScriptObject( py_entity );

        entity->setBehavior( behavior );

        entity->onCreate();

        return entity;
    }
}