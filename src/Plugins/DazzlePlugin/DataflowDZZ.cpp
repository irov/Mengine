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
#include "Kernel/MemoryCopy.h"

#include "dazzle/dazzle_read.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct dz_stream_t
    {
        const void * data;
    };
    //////////////////////////////////////////////////////////////////////////
    static dz_result_t s_dz_stream_read( void * const _data, dz_size_t _size, dz_userdata_t _ud )
    {
        dz_stream_t * stream = reinterpret_cast<dz_stream_t *>(_ud);

        Helper::memoryCopy( _data, 0U, stream->data, _size );

        stream->data = (const dz_uint8_t *)stream->data + _size;

        return DZ_SUCCESSFUL;
    }
    //////////////////////////////////////////////////////////////////////////
    DataflowDZZ::DataflowDZZ()
        : m_service( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DataflowDZZ::~DataflowDZZ()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DataflowDZZ::setDazzleService( const dz_service_t * _service )
    {
        m_service = _service;
    }
    //////////////////////////////////////////////////////////////////////////
    const dz_service_t * DataflowDZZ::getDazzleService() const
    {
        return m_service;
    }
    //////////////////////////////////////////////////////////////////////////
    void DataflowDZZ::setArchivator( const ArchivatorInterfacePtr & _archivator )
    {
        m_archivator = _archivator;
    }
    //////////////////////////////////////////////////////////////////////////
    const ArchivatorInterfacePtr & DataflowDZZ::getArchivator() const
    {
        return m_archivator;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DataflowDZZ::initialize()
    {
        m_factoryDazzleData = Helper::makeFactoryPool<DazzleData, 128>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DataflowDZZ::finalize()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryDazzleData );

        m_factoryDazzleData = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DataflowDZZ::isThreadFlow() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    DataInterfacePtr DataflowDZZ::create( const DocumentPtr & _doc )
    {
        DazzleDataPtr data = m_factoryDazzleData->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( data );

        data->setDazzleService( m_service );

        return data;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr DataflowDZZ::load( const InputStreamInterfacePtr & _stream, const DocumentPtr & _doc )
    {
        MemoryInterfacePtr memory = Helper::loadStreamArchiveData( _stream, m_archivator, GET_MAGIC_NUMBER( MAGIC_DZZ ), GET_MAGIC_VERSION( MAGIC_DZZ ), _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memory );

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DataflowDZZ::flow( const DataInterfacePtr & _data, const MemoryInterfacePtr & _memory, const DataflowContext * _context, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _doc );

        DazzleData * data = _data.getT<DazzleData *>();

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
    //////////////////////////////////////////////////////////////////////////
}