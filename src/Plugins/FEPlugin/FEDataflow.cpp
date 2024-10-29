#include "FEDataflow.h"

#include "FEData.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/StreamHelper.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FEDataflow::FEDataflow()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FEDataflow::~FEDataflow()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool FEDataflow::initialize()
    {
        m_factoryFEData = Helper::makeFactoryPool<FEData, 128>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FEDataflow::finalize()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryFEData );

        m_factoryFEData = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FEDataflow::isThreadFlow() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    DataInterfacePtr FEDataflow::create( const DocumentInterfacePtr & _doc )
    {
        FEDataPtr data = m_factoryFEData->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( data, "invalid create data" );

        return data;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr FEDataflow::load( const InputStreamInterfacePtr & _stream, const DocumentInterfacePtr & _doc )
    {
        MemoryInterfacePtr memory = Helper::createMemoryStream( _stream, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid create memory" );

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FEDataflow::flow( const DataInterfacePtr & _data, const MemoryInterfacePtr & _memory, const DataflowContext * _context, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _doc );

        FEData * data = _data.getT<FEData *>();

        const void * memory_buffer = _memory->getBuffer();
        size_t memory_size = _memory->getSize();

        fe_bundle * bundle = fe_bundle_load( memory_buffer, (int32_t)memory_size );

        MENGINE_ASSERTION_MEMORY_PANIC( bundle, "invalid load fe bundle" );

        data->setFEBundle( bundle );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}