#include "EntityPrototypeGenerator.h"
#include "PythonEntityBehavior.h"

#include "Environment/Python/PythonEventReceiver.h"

#include "Interface/StringizeInterface.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/FactoryAssertion.h"
#include "Kernel/Logger.h"

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
    }
    //////////////////////////////////////////////////////////////////////////
    EntityPrototypeGenerator::EntityPrototypeGenerator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EntityPrototypeGenerator::~EntityPrototypeGenerator()
    {
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

        m_generator = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    FactoryPtr EntityPrototypeGenerator::_initializeFactory()
    {
        FactoryPtr factory = new FactoryPool<PythonEntityBehavior, 128>();

        return factory;
    }
    //////////////////////////////////////////////////////////////////////////
    pybind::object EntityPrototypeGenerator::preparePythonType()
    {
        if( m_type.is_invalid() == false )
        {
            return m_type;
        }

        pybind::object py_type = m_generator.call( m_prototype );

        if( py_type.is_invalid() == true || py_type.is_none() == true )
        {
            LOGGER_ERROR( "PythonPrototypeGenerator type %s prototype %s invalid type create"
                , m_category.c_str()
                , m_prototype.c_str()
            );

            return pybind::make_invalid_object_t();
        }

        if( py_type.is_type_class() == true )
        {
            PyObject * py_type_ptr = py_type.ptr();
            pybind::kernel_interface * py_kernel = py_type.kernel();

            if( py_kernel->type_initialize( py_type_ptr ) == false )
            {
                LOGGER_ERROR( "PythonPrototypeGenerator prototype %s invalid type initialize"
                    , m_category.c_str()
                    , m_prototype.c_str()
                );

                return pybind::make_invalid_object_t();
            }
        }

        Helper::registerScriptEventReceiverMethod<PythonEntityEventReceiver>( py_type, this, "onCreate", EVENT_ENTITY_CREATE );
        Helper::registerScriptEventReceiverMethod<PythonEntityEventReceiver>( py_type, this, "onDestroy", EVENT_ENTITY_DESTROY );
        Helper::registerScriptEventReceiverMethod<PythonEntityEventReceiver>( py_type, this, "onPreparation", EVENT_ENTITY_PREPARATION );
        Helper::registerScriptEventReceiverMethod<PythonEntityEventReceiver>( py_type, this, "onActivate", EVENT_ENTITY_ACTIVATE );
        Helper::registerScriptEventReceiverMethod<PythonEntityEventReceiver>( py_type, this, "onPreparationDeactivate", EVENT_ENTITY_PREPARATION_DEACTIVATE );
        Helper::registerScriptEventReceiverMethod<PythonEntityEventReceiver>( py_type, this, "onDeactivate", EVENT_ENTITY_DEACTIVATE );
        Helper::registerScriptEventReceiverMethod<PythonEntityEventReceiver>( py_type, this, "onCompile", EVENT_ENTITY_COMPILE );
        Helper::registerScriptEventReceiverMethod<PythonEntityEventReceiver>( py_type, this, "onRelease", EVENT_ENTITY_RELEASE );

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
    FactorablePointer EntityPrototypeGenerator::generate()
    {
        const pybind::object & py_type = this->preparePythonType();

        if( py_type.is_invalid() == true )
        {
            return nullptr;
        }

        pybind::object py_entity = py_type.call();

        if( py_entity.is_invalid() == true )
        {
            LOGGER_ERROR( "EntityPrototypeGenerator.generate: can't create object '%s' '%s' (invalid create)"
                , m_category.c_str()
                , m_prototype.c_str()
            );

            return nullptr;
        }

        EntityPtr entity = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), m_category );

        if( entity == nullptr )
        {
            LOGGER_ERROR( "EntityPrototypeGenerator can't generate '%s' '%s'"
                , m_category.c_str()
                , m_prototype.c_str()
            );

            return nullptr;
        }

        entity->setPrototype( m_prototype );

        entity->setBehaviorEventable( this );

        const FactoryPtr & factory = this->getFactory();

        PythonEntityBehaviorPtr behavior = factory->createObject();
        behavior->setScriptObject( py_entity );

        entity->setBehavior( behavior );

        entity->onCreate();

        return entity;
    }
}