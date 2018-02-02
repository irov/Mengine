#	include "DataManager.h"

#	include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( DataService, Menge::DataManager );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	DataManager::DataManager()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	DataManager::~DataManager()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void DataManager::registerDataflow( const ConstString& _type, const DataflowInterfacePtr & _dataflow )
	{
		m_dataflows.insert( std::make_pair( _type, _dataflow ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DataManager::unregisterDataflow( const ConstString& _type )
	{
		m_dataflows.erase( _type );
	}
	//////////////////////////////////////////////////////////////////////////
	DataflowInterfacePtr DataManager::getDataflow( const ConstString & _type ) const
	{
		TMapDataflow::const_iterator it_found = m_dataflows.find( _type );

		if( it_found == m_dataflows.end() )
		{
			LOGGER_ERROR("DataManager::getDataflow '%s' don't register"
				, _type.c_str()
				);

			return nullptr;
		}

		DataflowInterfacePtr dataflow = it_found->second;

		return dataflow;
	}
	//////////////////////////////////////////////////////////////////////////
	DataInterfacePtr DataManager::dataflow( const ConstString & _type, const InputStreamInterfacePtr & _stream )
	{
		TMapDataflow::const_iterator it_found = m_dataflows.find( _type );

		if( it_found == m_dataflows.end() )
		{
			LOGGER_ERROR("DataManager::dataflow '%s' don't register"
				, _type.c_str()
				);

			return nullptr;
		}

		DataflowInterfacePtr dataflow = it_found->second;

		DataInterfacePtr data = dataflow->create();

		if( data == nullptr )
		{
			LOGGER_ERROR("DataManager::dataflow '%s' invalid create data"
				, _type.c_str()
				);

			return nullptr;
		}

		if( dataflow->load( data, _stream ) == false )
		{
			LOGGER_ERROR("DataManager::dataflow '%s' invalid load data"
				, _type.c_str()
				);

			return nullptr;
		}

		return data;		
	}
}