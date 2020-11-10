#include "PrototypeService.h"

#include "Interface/ConfigServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PrototypeService, Mengine::PrototypeService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PrototypeService::PrototypeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PrototypeService::~PrototypeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrototypeService::_initializeService()
    {
        uint32_t PrototypeHashTableSize = CONFIG_VALUE( "Engine", "PrototypeHashTableSize", 1024 );

        m_generators.reserve( PrototypeHashTableSize );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PrototypeService::_finalizeService()
    {
        for( const HashtablePrototypes::value_type & value : m_generators )
        {
            const PrototypeGeneratorInterfacePtr & generator = value.element;

            const ConstString & category = value.key1;
            const ConstString & prototype = value.key1;

            LOGGER_ERROR( "Forgot remove '%s' generator '%s'"
                , category.c_str()
                , prototype.c_str()
            );

            generator->finalize();
        }

        m_generators.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrototypeService::addPrototype( const ConstString & _category, const ConstString & _prototype, const PrototypeGeneratorInterfacePtr & _generator )
    {
        _generator->setCategory( _category );
        _generator->setPrototype( _prototype );

        if( _generator->initialize() == false )
        {
            LOGGER_ERROR( "add '%s:%s' invalid initialize!"
                , _category.c_str()
                , _prototype.c_str()
            );

            return false;
        }

        MENGINE_ASSERTION_FATAL( m_generators.exist( _category, _prototype ) == false, "prototype '%s:%s' alredy exist"
            , _category.c_str()
            , _prototype.c_str()
        );

        m_generators.emplace( _category, _prototype, _generator );

        LOGGER_INFO( "prototype", "add '%s:%s'"
            , _category.c_str()
            , _prototype.c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PrototypeService::removePrototype( const ConstString & _category, const ConstString & _prototype )
    {
        PrototypeGeneratorInterfacePtr generator = m_generators.erase( _category, _prototype );

        MENGINE_ASSERTION_MEMORY_PANIC( generator, "not found '%s' prototype '%s'"
            , _category.c_str()
            , _prototype.c_str()
        );

        generator->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrototypeService::hasGenerator( const ConstString & _category, const ConstString & _prototype, PrototypeGeneratorInterfacePtr * const _generator ) const
    {
        const PrototypeGeneratorInterfacePtr & generator = m_generators.find( _category, _prototype );

        if( generator == nullptr )
        {
            return false;
        }

        if( _generator != nullptr )
        {
            *_generator = generator;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const PrototypeGeneratorInterfacePtr & PrototypeService::getGenerator( const ConstString & _category, const ConstString & _prototype ) const
    {
        const PrototypeGeneratorInterfacePtr & generator = m_generators.find( _category, _prototype );

        MENGINE_ASSERTION_MEMORY_PANIC( generator, "prototype not found '%s' generator '%s'"
            , _category.c_str()
            , _prototype.c_str()
        );

        return generator;
    }
    //////////////////////////////////////////////////////////////////////////
    FactorablePointer PrototypeService::generatePrototype( const ConstString & _category, const ConstString & _prototype, const DocumentPtr & _doc )
    {
        const PrototypeGeneratorInterfacePtr & generator = m_generators.find( _category, _prototype );

        MENGINE_ASSERTION_MEMORY_PANIC( generator, "prototype not found '%s:%s' doc '%s'"
            , _category.c_str()
            , _prototype.c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        FactorablePtr prototype = generator->generate( _doc );

        return prototype;
    }
    //////////////////////////////////////////////////////////////////////////
    void PrototypeService::foreachGenerators( const LambdaPrototypeGenerator & _lambda ) const
    {
        for( const HashtablePrototypes::value_type & value : m_generators )
        {
            const PrototypeGeneratorInterfacePtr & generator = value.element;

            _lambda( generator );
        }
    }
}