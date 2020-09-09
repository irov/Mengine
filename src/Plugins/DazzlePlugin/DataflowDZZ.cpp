#include "DataflowDZZ.h"

#include "DazzleInterface.h"

#include "DazzleData.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Stream.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"

#include "stdex/memorycopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct dz_stream_t
    {        
        const void * data;
    };
    //////////////////////////////////////////////////////////////////////////
    static dz_result_t s_dz_stream_read( void * _data, dz_size_t _size, dz_userdata_t _ud )
    {
        dz_stream_t * stream = reinterpret_cast<dz_stream_t *>(_ud);

        stdex::memorycopy( _data, 0U, stream->data, _size );

        stream->data = (const dz_uint8_t *)stream->data + _size;

        return DZ_SUCCESSFUL;
    }
    //////////////////////////////////////////////////////////////////////////
    DataflowDDZ::DataflowDDZ()
        : m_service( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DataflowDDZ::~DataflowDDZ()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DataflowDDZ::setDazzleService( const dz_service_t * _service )
    {
        m_service = _service;
    }
    //////////////////////////////////////////////////////////////////////////
    const dz_service_t * DataflowDDZ::getDazzleService() const
    {
        return m_service;
    }
    //////////////////////////////////////////////////////////////////////////
    void DataflowDDZ::setArchivator( const ArchivatorInterfacePtr & _archivator )
    {
        m_archivator = _archivator;
    }
    //////////////////////////////////////////////////////////////////////////
    const ArchivatorInterfacePtr & DataflowDDZ::getArchivator() const
    {
        return m_archivator;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DataflowDDZ::initialize()
    {
        m_factoryDazzleData = Helper::makeFactoryPool<DazzleData, 128>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DataflowDDZ::finalize()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryDazzleData );

        m_factoryDazzleData = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DataflowDDZ::isThreadFlow() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    DataInterfacePtr DataflowDDZ::create( const DocumentPtr & _doc )
    {
        DazzleDataPtr data = m_factoryDazzleData->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( data );

        return data;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr DataflowDDZ::load( const InputStreamInterfacePtr & _stream, const DocumentPtr & _doc )
    {
        MemoryInterfacePtr memory = Helper::loadStreamArchiveData( _stream, m_archivator, GET_MAGIC_NUMBER( MAGIC_AEZ ), GET_MAGIC_VERSION( MAGIC_AEZ ), _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memory );

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DataflowDDZ::flow( const DataInterfacePtr & _data, const MemoryInterfacePtr & _memory, const DataflowContext * _context, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _doc );

        DazzleData * data = stdex::intrusive_get<DazzleData *>( _data );

        void * memory_buffer = _memory->getBuffer();

        dz_stream_t stream;
        stream.data = memory_buffer;

        dz_effect_read_status_e status;
        if( dz_header_read( &s_dz_stream_read, &stream, &status ) == DZ_FAILURE )
        {
            return false;
        }

        if( status != DZ_EFFECT_LOAD_STATUS_SUCCESSFUL )
        {
            return false;
        }

        dz_effect_t * effect;
        if( dz_effect_read( m_service, &effect, &s_dz_stream_read, &stream ) == DZ_FAILURE )
        {
            return false;
        }

        data->setDazzleEffect( effect );

        return true;
    }
}