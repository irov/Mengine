#include "DataService.h"

#include "Interface/VocabularyServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionVocabulary.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileStreamHelper.h"

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
    DataInterfacePointer DataService::dataflow( const DataflowInterfacePtr & _dataflow, const InputStreamInterfacePtr & _stream, const DataflowContext * _context, const DocumentInterfacePtr & _doc )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _dataflow );

        DataInterfacePtr data = _dataflow->create( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( data, "invalid create data '%s'"
            , MENGINE_DOCUMENT_STR( _doc )
        );

        MemoryInterfacePtr memory = _dataflow->load( _stream, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid load data (doc: %s)"
            , MENGINE_DOCUMENT_STR( _doc )
        );

        if( _dataflow->flow( data, memory, _context, _doc ) == false )
        {
            LOGGER_ERROR( "invalid flow data (doc: %s)"
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return nullptr;
        }

        return data;
    }
    //////////////////////////////////////////////////////////////////////////
}