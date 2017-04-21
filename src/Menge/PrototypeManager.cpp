#	include "PrototypeManager.h"

#	include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PrototypeService, Menge::PrototypeManager );
//////////////////////////////////////////////////////////////////////////
namespace Menge
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
    bool PrototypeManager::_initialize()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PrototypeManager::_finalize()
    {
        for( size_t index = 0; index != MENGINE_PROTOTYPE_HASH_SIZE; ++index )
        {
            TVectorPrototypes & prototypes = m_prototypes[index];

            prototypes.clear();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    static uint32_t getPrototypeHashId( const ConstString & _category, const ConstString & _prototype )
    {
        ConstString::hash_type hash = _category.hash() + _prototype.hash();
        uint32_t hash_id = (uint32_t)hash % MENGINE_PROTOTYPE_HASH_SIZE;

        return hash_id;
    }
	//////////////////////////////////////////////////////////////////////////
	bool PrototypeManager::addPrototype( const ConstString & _category, const ConstString & _prototype, const PrototypeGeneratorInterfacePtr & _generator )
	{
		_generator->setServiceProvider( m_serviceProvider );

		if( _generator->initialize( _category, _prototype ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("PrototypeManager::addPrototype add %s:%s invalid initialize!"
				, _category.c_str()
				, _prototype.c_str()
				);

			return false;
		}

        uint32_t hash_id = getPrototypeHashId( _category, _prototype );

        TVectorPrototypes & prototypes = m_prototypes[hash_id];

        CategoryKey key;
        key.category = _category;
        key.prototype = _prototype;
        key.generator = _generator;

        prototypes.push_back( key );

        LOGGER_INFO(m_serviceProvider)("PrototypeManager::addPrototype add %s:%s"
            , _category.c_str()
            , _prototype.c_str()
            );

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    bool PrototypeManager::removePrototype( const ConstString & _category, const ConstString & _prototype )
    {
        uint32_t hash_id = getPrototypeHashId( _category, _prototype );

        TVectorPrototypes & prototypes = m_prototypes[hash_id];

        for( TVectorPrototypes::iterator
            it = prototypes.begin(),
            it_end = prototypes.end();
            it != it_end;
            ++it )
        {
            const CategoryKey & key = *it;

            if( key.category != _category ||
                key.prototype != _prototype )
            {
                continue;
            }

            *it = prototypes.back();
            prototypes.pop_back();

            return true;
        }

        return false;
    }
	//////////////////////////////////////////////////////////////////////////
	bool PrototypeManager::hasPrototype( const ConstString & _category, const ConstString & _prototype, PrototypeGeneratorInterfacePtr & _generator ) const
	{
        uint32_t hash_id = getPrototypeHashId( _category, _prototype );

        const TVectorPrototypes & prototypes = m_prototypes[hash_id];

		for( TVectorPrototypes::const_iterator
            it = prototypes.begin(),
            it_end = prototypes.end();
            it != it_end;
            ++it )
		{
            const CategoryKey & key = *it;

            if( key.category != _category ||
                key.prototype != _prototype )
            {
                continue;
            }

            _generator = key.generator;

            return true;
		}

		return false;
	}
    //////////////////////////////////////////////////////////////////////////
	PointerFactorable PrototypeManager::generatePrototype( const ConstString & _category, const ConstString & _prototype )
    {
        PrototypeGeneratorInterfacePtr generator;
        if( this->hasPrototype( _category, _prototype, generator ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("PrototypeManager::generatePrototype prototype %s:%s not found"
                , _category.c_str()
                , _prototype.c_str()
                );

            return nullptr;
        }

        Factorable * prototype = generator->generate();

        return prototype;
    }
    //////////////////////////////////////////////////////////////////////////
    void PrototypeManager::visitGenerators( VisitorPrototypeGenerator * _visitor ) const
    {
        for( size_t index = 0; index != MENGINE_PROTOTYPE_HASH_SIZE; ++index )
        {
            const TVectorPrototypes & prototypes = m_prototypes[index];

            for( TVectorPrototypes::const_iterator
                it = prototypes.begin(),
                it_end = prototypes.end();
                it != it_end;
                ++it )
            {
                const CategoryKey & key = *it;

                _visitor->visit( key.category, key.prototype, key.generator );
            }
        }
    }
}