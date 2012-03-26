#	include "ConverterEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ConverterEngine::ConverterEngine()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ConverterEngine::~ConverterEngine()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ConverterEngine::registerConverter( const ConstString& _type, ConverterSystemInterface * _interface )
	{
		m_mapConverterSystem.insert( std::make_pair(_type, _interface) );
		_interface->setService( this );
	}
	//////////////////////////////////////////////////////////////////////////
	void ConverterEngine::unregisterConverter( const ConstString& _type )
	{
		ConstString c_type(_type);
		m_mapConverterSystem.erase( c_type );
	}
	//////////////////////////////////////////////////////////////////////////
	ConverterInterface * ConverterEngine::createConverter( const ConstString & _type )
	{
		TMapConverterSystem::iterator it_find = m_mapConverterSystem.find( _type );

		if( it_find == m_mapConverterSystem.end() )
		{
			return 0;
		}

		ConverterInterface * decoder = 
			it_find->second->createConverter();

		if( decoder == 0 )
		{
			return 0;
		}

		if( decoder->initialize() == false )
		{
			decoder->destroy();

			return 0;
		}

		return decoder;
	}
	//////////////////////////////////////////////////////////////////////////
}
