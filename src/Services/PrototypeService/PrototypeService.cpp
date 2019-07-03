#include "PrototypeService.h"

#include "Interface/ConfigServiceInterface.h"

#include "Kernel/Logger.h"

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

        CategoryKey key;
        key.hash = _category.hash() + _prototype.hash();
        key.category = _category;
        key.prototype = _prototype;

        m_generators.emplace( key, _generator );

        LOGGER_INFO( "add '%s:%s'"
            , _category.c_str()
            , _prototype.c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrototypeService::removePrototype( const ConstString & _category, const ConstString & _prototype )
    {
        CategoryKey key;
        key.hash = _category.hash() + _prototype.hash();
        key.category = _category;
        key.prototype = _prototype;

        PrototypeGeneratorInterfacePtr generator = m_generators.erase( key );

        if( generator == nullptr )
        {
            return false;
        }

        generator->finalize();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const PrototypeGeneratorInterfacePtr & PrototypeService::getGenerator( const ConstString & _category, const ConstString & _prototype ) const
    {
        CategoryKey key;
        key.hash = _category.hash() + _prototype.hash();
        key.category = _category;
        key.prototype = _prototype;

        const PrototypeGeneratorInterfacePtr & generator = m_generators.find( key );

        if( generator == nullptr )
        {
            LOGGER_ERROR( "prototype not found '%s:%s'"
                , _category.c_str()
                , _prototype.c_str()
            );

            return PrototypeGeneratorInterfacePtr::none();
        }

        return generator;
    }
    //////////////////////////////////////////////////////////////////////////
    FactorablePointer PrototypeService::generatePrototype( const ConstString & _category, const ConstString & _prototype, const Char * _doc )
    {
        CategoryKey key;
        key.hash = _category.hash() + _prototype.hash();
        key.category = _category;
        key.prototype = _prototype;

        const PrototypeGeneratorInterfacePtr & generator = m_generators.find( key );

        if( generator == nullptr )
        {
            LOGGER_ERROR( "prototype not found '%s:%s' doc '%s'"
                , _category.c_str()
                , _prototype.c_str()
                , _doc
            );

            return nullptr;
        }

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