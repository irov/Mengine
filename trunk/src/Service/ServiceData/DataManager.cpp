#	include "DataManager.h"

#	include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( DataService, Menge::DataServiceInterface, Menge::DataManager );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	DataManager::DataManager()
		: m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	DataManager::~DataManager()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool DataManager::initialize()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void DataManager::finalize()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void DataManager::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * DataManager::getServiceProvider() const
	{
		return m_serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	void DataManager::registerDataflow( const ConstString& _type, const DataflowInterfacePtr & _dataflow )
	{
		m_dataflows.insert( _type, _dataflow );
	}
	//////////////////////////////////////////////////////////////////////////
	void DataManager::unregisterDataflow( const ConstString& _type )
	{
		m_dataflows.erase( _type );
	}
	//////////////////////////////////////////////////////////////////////////
	DataflowInterfacePtr DataManager::getDataflow( const ConstString & _type ) const
	{
		DataflowInterfacePtr dataflow;
		if( m_dataflows.has_copy( _type, dataflow ) == false )
		{	
			LOGGER_ERROR(m_serviceProvider)("DataManager::getDataflow '%s' don't register"
				, _type.c_str()
				);

			return nullptr;
		}

		return dataflow;
	}
	//////////////////////////////////////////////////////////////////////////
	DataInterfacePtr DataManager::dataflow( const ConstString & _type, const InputStreamInterfacePtr & _stream )
	{
		DataflowInterfacePtr dataflow;
		if( m_dataflows.has_copy( _type, dataflow ) == false )
		{	
			LOGGER_ERROR(m_serviceProvider)("DataManager::dataflow '%s' don't register"
				, _type.c_str()
				);

			return nullptr;
		}

		DataInterfacePtr data = dataflow->load( _stream );

		return data;		
	}
}