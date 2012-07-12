#	include "PrototypeManager.h"

#	include "LogEngine.h"

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
			return false;
		}

		pybind::incref( _module );

		TMapPrototypes::iterator it_found = m_prototypes.find( _prototype );

		if( it_found != m_prototypes.end() )
		{
			MENGE_LOG_INFO("PrototypeManager::addPrototype already exist prototype %s (override)"
				, _prototype.c_str()
				);

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
			return 0;
		}

		PyObject * module = it_found->second;

		return module;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PrototypeManager::hasPrototype( const ConstString & _prototype ) const
	{
		TMapPrototypes::const_iterator it_found = m_prototypes.find( _prototype );

		if( it_found == m_prototypes.end() )
		{
			return false;
		}

		return true;
	}
}