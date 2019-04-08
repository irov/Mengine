#include "FEDataflow.h"

#include "Interface/MemoryInterface.h"

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
    DataInterfacePtr FEDataflow::create( const Char * _doc )
    {
        FEDataPtr data = m_factoryFEData->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( data, nullptr );

        return data;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FEDataflow::load( const DataInterfacePtr & _data, const InputStreamInterfacePtr & _stream, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        FEData * data = stdex::intrusive_get<FEData *>( _data );

        MemoryInterfacePtr memory = Helper::createMemoryStream( _stream, "FEDataflow::load", __FILE__, __LINE__ );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, false );

        const void * memory_buffer = memory->getBuffer();
        size_t memory_size = memory->getSize();

        fe_bundle * bundle = fe_bundle_load( memory_buffer, (int32_t)memory_size );

        if( bundle == nullptr )
        {
            return false;
        }

        data->setFEBundle( bundle );

        return true;
    }
}