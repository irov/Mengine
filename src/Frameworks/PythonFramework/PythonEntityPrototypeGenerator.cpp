#include "PythonEntityPrototypeGenerator.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/DocumentInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonEventReceiver.h"

#include "PythonEntityBehavior.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/PrototypeHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonEntityPrototypeGenerator::PythonEntityPrototypeGenerator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonEntityPrototypeGenerator::~PythonEntityPrototypeGenerator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonEntityPrototypeGenerator::setGenerator( const pybind::object & _generator )
    {
        m_generator = _generator;
    }
    //////////////////////////////////////////////////////////////////////////
    const pybind::object & PythonEntityPrototypeGenerator::getGenerator() const
    {
        return m_generator;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonEntityPrototypeGenerator::initialize()
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
    void PythonEntityPrototypeGenerator::finalize()
    {
        FactoryPrototypeGenerator::finalize();

        if( m_eventable != nullptr )
        {
            EventationInterface * eventation = m_eventable->getEventation();
            eventation->removeEvents();

            m_eventable = nullptr;
        }

        m_generator = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    FactoryInterfacePtr PythonEntityPrototypeGenerator::_initializeFactory()
    {
        FactoryInterfacePtr factory = Helper::makeFactoryPool<PythonEntityBehavior, 128>( MENGINE_DOCUMENT_FACTORABLE );

        return factory;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonEntityPrototypeGenerator::_finalizeFactory()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonEntityPrototypeGenerator::setPythonType( const pybind::object & _type )
    {
        m_type = _type;
    }
    //////////////////////////////////////////////////////////////////////////
    const pybind::object & PythonEntityPrototypeGenerator::getPythonType()
    {
        if( m_type.is_invalid() == false )
        {
            return m_type;
        }

        pybind::object py_type = m_generator.call( m_prototype );

        EventablePtr eventable = SCRIPT_SERVICE()
            ->eventableEntity( py_type );

        MENGINE_ASSERTION_MEMORY_PANIC( eventable, "type '%s' prototype '%s' invalid eventable entity"
            , m_category.c_str()
            , m_prototype.c_str()
        );

        m_eventable = eventable;

        m_type = py_type;

        return m_type;
    }
    //////////////////////////////////////////////////////////////////////////
    FactorablePointer PythonEntityPrototypeGenerator::generate( const DocumentInterfacePtr & _doc )
    {
        const pybind::object & py_type = this->getPythonType();

        if( py_type.is_invalid() == true )
        {
            return nullptr;
        }

        pybind::object py_entity = py_type.call();

        if( py_entity.is_invalid() == true )
        {
            LOGGER_ERROR( "can't create object '%s' '%s' (invalid create) (doc: %s)"
                , m_category.c_str()
                , m_prototype.c_str()
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return nullptr;
        }

        EntityPtr entity = Helper::generatePrototype( Node::getFactorableType(), m_category, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( entity, "can't generate '%s' '%s' doc '%s'"
            , m_category.c_str()
            , m_prototype.c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        entity->setPrototype( m_prototype );

        entity->setBehaviorEventable( m_eventable );

        const FactoryInterfacePtr & factory = this->getPrototypeFactory();

        PythonEntityBehaviorPtr behavior = factory->createObject( _doc );
        behavior->setScriptObject( py_entity );

        entity->setBehavior( behavior );

        if( entity->create() == false )
        {
            LOGGER_ERROR( "invalid create entity category '%s' prototype '%s' (doc: %s)"
                , m_category.c_str()
                , m_prototype.c_str()
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return nullptr;
        }

        return entity;
    }
}