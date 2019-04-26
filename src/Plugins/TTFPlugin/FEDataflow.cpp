#include "FEDataflow.h"

#include "FEData.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Stream.h"
#include "Kernel/MemoryHelper.h"
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
        m_factoryFEData = new FactoryPool<FEData, 128>();

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
    DataInterfacePtr FEDataflow::create( const Char * _doc )
    {
        FEDataPtr data = m_factoryFEData->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( data, nullptr );

        return data;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr FEDataflow::load( const InputStreamInterfacePtr & _stream, const Char * _doc )
    {
        MemoryInterfacePtr memory = Helper::createMemoryStream( _stream, _doc, __FILE__, __LINE__ );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, nullptr );

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FEDataflow::flow( const DataInterfacePtr & _data, const MemoryInterfacePtr & _memory, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        FEData * data = stdex::intrusive_get<FEData *>( _data );

        const void * memory_buffer = _memory->getBuffer();
        size_t memory_size = _memory->getSize();

        fe_bundle * bundle = fe_bundle_load( memory_buffer, (int32_t)memory_size );

        if( bundle == nullptr )
        {
            return false;
        }

        data->setFEBundle( bundle );

        return true;
    }
}