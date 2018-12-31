#include "DataService.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionVocabulary.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( DataService, Mengine::DataService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DataService::DataService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DataService::~DataService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DataService::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DataService::_finalizeService()
    {
        MENGINE_ASSERTION_VOCABULARY_EMPTY( STRINGIZE_STRING_LOCAL( "Dataflow" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    DataInterfacePtr DataService::dataflow( const DataflowInterfacePtr & _dataflow, const InputStreamInterfacePtr & _stream, const Char * _doc )
    {
        DataInterfacePtr data = _dataflow->create();

        if( data == nullptr )
        {
            LOGGER_ERROR( "invalid create data"
            );

            return nullptr;
        }

        if( _dataflow->load( data, _stream, _doc ) == false )
        {
            LOGGER_ERROR( "invalid load data"
            );

            return nullptr;
        }

        return data;
    }
}