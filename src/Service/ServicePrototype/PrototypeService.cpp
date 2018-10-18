#include "PrototypeService.h"

#include "Interface/ConfigInterface.h"

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

        m_prototypes.reserve( PrototypeHashTableSize );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PrototypeService::_finalizeService()
    {
        m_prototypes.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrototypeService::addPrototype( const ConstString & _category, const ConstString & _prototype, const PrototypeGeneratorInterfacePtr & _generator )
    {
        _generator->setCategory( _category );
        _generator->setPrototype( _prototype );

        if( _generator->initialize() == false )
        {
            LOGGER_ERROR( "PrototypeManager::addPrototype add '%s:%s' invalid initialize!"
                , _category.c_str()
                , _prototype.c_str()
            );

            return false;
        }

        CategoryKey key;
        key.category = _category;
        key.prototype = _prototype;

        m_prototypes.insert( key, _generator );

        LOGGER_INFO( "PrototypeManager::addPrototype add '%s:%s'"
            , _category.c_str()
            , _prototype.c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrototypeService::removePrototype( const ConstString & _category, const ConstString & _prototype )
    {
        CategoryKey key;
        key.category = _category;
        key.prototype = _prototype;

        m_prototypes.remove( key );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    const PrototypeGeneratorInterfacePtr & PrototypeService::getGenerator( const ConstString & _category, const ConstString & _prototype ) const
    {
        CategoryKey key;
        key.category = _category;
        key.prototype = _prototype;

        const PrototypeGeneratorInterfacePtr & generator = m_prototypes.find( key );

        return generator;
    }
    //////////////////////////////////////////////////////////////////////////
    PointerFactorable PrototypeService::generatePrototype( const ConstString & _category, const ConstString & _prototype )
    {
        CategoryKey key;
        key.category = _category;
        key.prototype = _prototype;

        const PrototypeGeneratorInterfacePtr & generator = m_prototypes.find( key );

        if( generator == nullptr )
        {
            LOGGER_ERROR( "PrototypeManager::generatePrototype prototype '%s:%s' not found"
                , _category.c_str()
                , _prototype.c_str()
            );

            return nullptr;
        }

        FactorablePtr prototype = generator->generate();

        return prototype;
    }
    //////////////////////////////////////////////////////////////////////////
    void PrototypeService::foreachGenerators( const LambdaPrototypeGenerator & _lambda ) const
    {
        for( const PrototypeGeneratorInterfacePtr & generator : m_prototypes )
        {
			_lambda( generator );
        }
    }
}