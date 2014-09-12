#	include "PrototypeManager.h"

#	include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PrototypeService, Menge::PrototypeServiceInterface, Menge::PrototypeManager );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PrototypeManager::PrototypeManager()
        : m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	PrototypeManager::~PrototypeManager()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void PrototypeManager::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * PrototypeManager::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool PrototypeManager::addPrototype( const ConstString & _category, const ConstString & _prototype, const PrototypeGeneratorInterfacePtr & _generator )
	{
        CategoryKey key;
        key.category = _category;
        key.prototype = _prototype;

		TMapPrototypes::insert_type it_insert = m_prototypes.insert( key, _generator );

		if( it_insert.second == false )
		{
			PrototypeGeneratorInterfacePtr conflict_generator = m_prototypes.get_value( it_insert.first );

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
				m_prototypes.set_value( it_insert.first, _generator );
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

        _generator = m_prototypes.get_value( it_found );

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    Factorable * PrototypeManager::generatePrototype( const ConstString & _category, const ConstString & _prototype )
    {
        CategoryKey key;
        key.category = _category;
        key.prototype = _prototype;

        PrototypeGeneratorInterfacePtr generator;
        if( this->hasPrototype( _category, _prototype, generator ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("PrototypeManager::generatePrototype prototype %s:%s not found"
                , _category.c_str()
                , _prototype.c_str()
                );

            return nullptr;
        }

        Factorable * prototype = generator->generate( _category, _prototype );

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
            const CategoryKey & keys = m_prototypes.get_key( it );
            const PrototypeGeneratorInterfacePtr & generator = m_prototypes.get_value( it );

            _visitor->visit( keys.category, keys.prototype, generator );
        }
    }
}