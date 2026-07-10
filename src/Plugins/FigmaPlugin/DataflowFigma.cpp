#include "DataflowFigma.h"

#include "FigmaData.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DataflowFigma::DataflowFigma()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DataflowFigma::~DataflowFigma()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DataflowFigma::initialize()
    {
        m_factoryFigmaData = Helper::makeFactoryPool<FigmaData, 16>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DataflowFigma::finalize()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryFigmaData );

        m_factoryFigmaData = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DataflowFigma::isThreadFlow() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    DataInterfacePtr DataflowFigma::create( const DocumentInterfacePtr & _doc )
    {
        FigmaDataPtr data = m_factoryFigmaData->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( data, "invalid create figma data" );

        return data;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr DataflowFigma::load( const InputStreamInterfacePtr & _stream, const DocumentInterfacePtr & _doc )
    {
        MemoryInterfacePtr memory = Helper::createMemoryStream( _stream, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid create figma memory" );

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DataflowFigma::flow( const DataInterfacePtr & _data, const MemoryInterfacePtr & _memory, const DataflowContext * _context, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        FigmaData * data = _data.getT<FigmaData *>();

        if( data->loadDocument( _memory, _context->filePath ) == false )
        {
            LOGGER_ERROR( "invalid load figma data '%s'"
                , _context->filePath.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
