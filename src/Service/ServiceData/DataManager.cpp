#include "DataManager.h"

#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( DataService, Mengine::DataManager );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
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
        m_dataflows.emplace( _type, _dataflow );
    }
    //////////////////////////////////////////////////////////////////////////
    void DataManager::unregisterDataflow( const ConstString& _type )
    {
        m_dataflows.erase( _type );
    }
    //////////////////////////////////////////////////////////////////////////
    const DataflowInterfacePtr & DataManager::getDataflow( const ConstString & _type ) const
    {
        MapDataflow::const_iterator it_found = m_dataflows.find( _type );

        if( it_found == m_dataflows.end() )
        {
            LOGGER_ERROR( "DataManager::getDataflow '%s' don't register"
                , _type.c_str()
            );

            return DataflowInterfacePtr::none();
        }

        const DataflowInterfacePtr & dataflow = it_found->second;

        return dataflow;
    }
    //////////////////////////////////////////////////////////////////////////
    DataInterfacePtr DataManager::dataflow( const DataflowInterfacePtr & _dataflow, const InputStreamInterfacePtr & _stream )
    {
        DataInterfacePtr data = _dataflow->create();

        if( data == nullptr )
        {
            LOGGER_ERROR( "DataManager::dataflow invalid create data"
            );

            return nullptr;
        }

        if( _dataflow->load( data, _stream ) == false )
        {
            LOGGER_ERROR( "DataManager::dataflow invalid load data"
            );

            return nullptr;
        }

        return data;
    }
}