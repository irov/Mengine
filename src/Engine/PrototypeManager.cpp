#include "PrototypeManager.h"

#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PrototypeService, Mengine::PrototypeManager );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PrototypeManager::PrototypeManager()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PrototypeManager::~PrototypeManager()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrototypeManager::_initializeService()
    {
        m_prototypes.reserve( 1024 );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PrototypeManager::_finalizeService()
    {
        m_prototypes.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrototypeManager::addPrototype( const ConstString & _category, const ConstString & _prototype, const PrototypeGeneratorInterfacePtr & _generator )
    {
        _generator->setCategory( _category );
        _generator->setPrototype( _prototype );

        if( _generator->initialize() == false )
        {
            LOGGER_ERROR( "PrototypeManager::addPrototype add %s:%s invalid initialize!"
                , _category.c_str()
                , _prototype.c_str()
            );

            return false;
        }

        CategoryKey key;
        key.category = _category;
        key.prototype = _prototype;

        m_prototypes.insert( key, _generator );

        LOGGER_INFO( "PrototypeManager::addPrototype add %s:%s"
            , _category.c_str()
            , _prototype.c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrototypeManager::removePrototype( const ConstString & _category, const ConstString & _prototype )
    {
        CategoryKey key;
        key.category = _category;
        key.prototype = _prototype;

        m_prototypes.remove( key );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    const PrototypeGeneratorInterfacePtr & PrototypeManager::getGenerator( const ConstString & _category, const ConstString & _prototype ) const
    {
        CategoryKey key;
        key.category = _category;
        key.prototype = _prototype;

        const PrototypeGeneratorInterfacePtr & generator = m_prototypes.find( key );

        return generator;
    }
    //////////////////////////////////////////////////////////////////////////
    PointerFactorable PrototypeManager::generatePrototype( const ConstString & _category, const ConstString & _prototype )
    {
        CategoryKey key;
        key.category = _category;
        key.prototype = _prototype;

        const PrototypeGeneratorInterfacePtr & generator = m_prototypes.find( key );

        if( generator == nullptr )
        {
            LOGGER_ERROR( "PrototypeManager::generatePrototype prototype %s:%s not found"
                , _category.c_str()
                , _prototype.c_str()
            );

            return nullptr;
        }

        FactorablePtr prototype = generator->generate();

        return prototype;
    }
    //////////////////////////////////////////////////////////////////////////
    void PrototypeManager::visitGenerators( VisitorPrototypeGenerator * _visitor ) const
    {
        for( const PrototypeGeneratorInterfacePtr & generator : m_prototypes )
        {
            _visitor->visit( generator );
        }
    }
}