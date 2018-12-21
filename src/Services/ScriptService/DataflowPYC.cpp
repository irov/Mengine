#include "DataflowPYC.h"

#include "Interface/ScriptServiceInterface.h"

#include "ScriptCodeData.h"

#include "Kernel/Stream.h"
#include "Kernel/MemoryHelper.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DataflowPYC::DataflowPYC()
        : m_kernel( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DataflowPYC::~DataflowPYC()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DataflowPYC::setKernel( pybind::kernel_interface * _kernel )
    {
        m_kernel = _kernel;
    }
    //////////////////////////////////////////////////////////////////////////
    pybind::kernel_interface * DataflowPYC::getKernel() const
    {
        return m_kernel;
    }
    //////////////////////////////////////////////////////////////////////////
    void DataflowPYC::setArchivator( const ArchivatorInterfacePtr & _archivator )
    {
        m_archivator = _archivator;
    }
    //////////////////////////////////////////////////////////////////////////
    const ArchivatorInterfacePtr & DataflowPYC::getArchivator() const
    {
        return m_archivator;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DataflowPYC::initialize()
    {
        m_factoryScriptCodeData = new FactoryPool<ScriptCodeData, 128>();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DataflowPYC::finalize()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryScriptCodeData );
        m_factoryScriptCodeData = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    DataInterfacePtr DataflowPYC::create()
    {
        ScriptCodeDataPtr data = m_factoryScriptCodeData->createObject();

        MENGINE_ASSERTION_MEMORY_PANIC( data, nullptr );

        return data;
    }
    //////////////////////////////////////////////////////////////////////////
    static int32_t s_get_int( const uint8_t * _buff )
    {
        int32_t x;
        x = (int32_t)_buff[0];
        x |= (int32_t)_buff[1] << 8;
        x |= (int32_t)_buff[2] << 16;
        x |= (int32_t)_buff[3] << 24;

        return x;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DataflowPYC::load( const DataInterfacePtr & _data, const InputStreamInterfacePtr & _stream )
    {
        ScriptCodeData * data = stdex::intrusive_get<ScriptCodeData *>( _data );

        MemoryInterfacePtr source_memory = Helper::loadStreamCacheArchiveMemory( _stream, m_archivator, "DataflowPYC", __FILE__, __LINE__ );

        MENGINE_ASSERTION_MEMORY_PANIC( source_memory, false );

        uint8_t * source_buffer = source_memory->getBuffer();
        size_t source_size = source_memory->getSize();

        MENGINE_ASSERTION_MEMORY_PANIC( source_buffer, false );

        long file_magic = s_get_int( source_buffer );
        long py_magic = m_kernel->marshal_magic_number();

        if( file_magic != py_magic )
        {
            LOGGER_ERROR( "module invalid magic '%u' need '%u'"
                , file_magic
                , py_magic
            );

            return false;
        }

        m_kernel->acquire_lock();

        PyObject * code = m_kernel->marshal_get_object( (char *)source_buffer + 8, source_size - 8 );

        if( code == nullptr )
        {
            LOGGER_ERROR( "module invalid marshal get object"
            );

            return false;
        }

#ifndef NDEBUG
        if( m_kernel->code_check( code ) == false )
        {
            LOGGER_ERROR( "module marshal get object not code"
            );

            return false;
        }
#endif

        m_kernel->release_lock();

        data->setScriptCode( pybind::make_borrowed_t( m_kernel, code ) );

        return true;
    }
}