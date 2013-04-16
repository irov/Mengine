#	include "PrototypeManager.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PrototypeManager::PrototypeManager()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	PrototypeManager::~PrototypeManager()
	{
		for( TMapPrototypes::iterator
			it = m_prototypes.begin(),
			it_end = m_prototypes.end();
		it != it_end;
		++it )
		{
			PyObject * module = it->second;

			pybind::decref( module );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool PrototypeManager::addPrototype( const ConstString & _prototype, PyObject * _module )
	{
		if( pybind::type_initialize( _module ) == false )
		{
			ServiceProviderInterface * serviceProvider = this->getServiceProvider();

			LOGGER_ERROR(serviceProvider)("PrototypeManager::addPrototype prototype %s invalid type initialize"
				, _prototype.c_str()
				);

			return false;
		}

		pybind::incref( _module );

		TMapPrototypes::iterator it_found = m_prototypes.find( _prototype );

		if( it_found != m_prototypes.end() )
		{
			m_prototypes[_prototype] = _module;
		}
		else
		{
			m_prototypes.insert( std::make_pair(_prototype, _module) );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * PrototypeManager::getPrototype( const ConstString & _prototype ) const
	{
		TMapPrototypes::const_iterator it_found = m_prototypes.find( _prototype );

		if( it_found == m_prototypes.end() )
		{
			ServiceProviderInterface * serviceProvider = this->getServiceProvider();

			LOGGER_ERROR(serviceProvider)("PrototypeManager::getPrototype prototype %s not found"
				, _prototype.c_str()
				);

			return 0;
		}

		PyObject * module = it_found->second;

		return module;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PrototypeManager::hasPrototype( const ConstString & _prototype, PyObject ** _module ) const
	{
		TMapPrototypes::const_iterator it_found = m_prototypes.find( _prototype );

		if( it_found == m_prototypes.end() )
		{
			return false;
		}

		if( _module != NULL )
		{
			*_module = it_found->second;
		}

		return true;
	}
}