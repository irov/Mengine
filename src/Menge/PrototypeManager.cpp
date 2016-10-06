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
	void PrototypeManager::_finalize()
	{ 
		m_prototypes.clear();
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

        CategoryKey key;
        key.category = _category;
        key.prototype = _prototype;

		std::pair<TMapPrototypes::iterator, bool> it_insert = m_prototypes.insert( std::make_pair( key, _generator ) );

		if( it_insert.second == false )
		{
			PrototypeGeneratorInterfacePtr conflict_generator = it_insert.first->second;

			if( conflict_generator->count() > 0 )
			{
				LOGGER_ERROR(m_serviceProvider)("PrototypeManager::addPrototype add %s:%s alredy exist and use!"
					, _category.c_str()
					, _prototype.c_str()
					);

				return false;
			}
			else
			{
				it_insert.first->second = _generator;
			}
		}

        LOGGER_INFO(m_serviceProvider)("PrototypeManager::addPrototype add %s:%s"
            , _category.c_str()
            , _prototype.c_str()
            );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PrototypeManager::hasPrototype( const ConstString & _category, const ConstString & _prototype, PrototypeGeneratorInterfacePtr & _generator ) const
	{
        CategoryKey prototype_key;
        prototype_key.category = _category;
        prototype_key.prototype = _prototype;

		TMapPrototypes::const_iterator it_found = m_prototypes.find( prototype_key );

		if( it_found == m_prototypes.end() )
		{
            CategoryKey category_key;
            category_key.category = _category;

            it_found = m_prototypes.find( category_key );
            
            if( it_found == m_prototypes.end() )
            {
                return false;
            }
		}

		_generator = it_found->second;

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    Factorable * PrototypeManager::generatePrototype( const ConstString & _category, const ConstString & _prototype )
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
        for( TMapPrototypes::const_iterator
            it = m_prototypes.begin(),
            it_end = m_prototypes.end();
        it != it_end;
        ++it )
        {
            const CategoryKey & keys = it->first;

            const PrototypeGeneratorInterfacePtr & generator = it->second;

            _visitor->visit( keys.category, keys.prototype, generator );
        }
    }
}