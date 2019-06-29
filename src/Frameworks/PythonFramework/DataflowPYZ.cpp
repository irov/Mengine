#include "DataflowPYZ.h"

#include "Interface/ScriptServiceInterface.h"

#include "PythonScriptCodeData.h"

#include "Kernel/Stream.h"
#include "Kernel/MemoryHelper.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DataflowPYZ::DataflowPYZ()
        : m_kernel( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DataflowPYZ::~DataflowPYZ()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DataflowPYZ::setKernel( pybind::kernel_interface * _kernel )
    {
        m_kernel = _kernel;
    }
    //////////////////////////////////////////////////////////////////////////
    pybind::kernel_interface * DataflowPYZ::getKernel() const
    {
        return m_kernel;
    }
    //////////////////////////////////////////////////////////////////////////
    void DataflowPYZ::setArchivator( const ArchivatorInterfacePtr & _archivator )
    {
        m_archivator = _archivator;
    }
    //////////////////////////////////////////////////////////////////////////
    const ArchivatorInterfacePtr & DataflowPYZ::getArchivator() const
    {
        return m_archivator;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DataflowPYZ::initialize()
    {
        m_factoryScriptCodeData = new FactoryPool<PythonScriptCodeData, 128>();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DataflowPYZ::finalize()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryScriptCodeData );
        m_factoryScriptCodeData = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DataflowPYZ::isThreadFlow() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    DataInterfacePtr DataflowPYZ::create( const Char * _doc )
    {
        ScriptCodeDataPtr data = m_factoryScriptCodeData->createObject( _doc );

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
    MemoryInterfacePtr DataflowPYZ::load( const InputStreamInterfacePtr & _stream, const Char * _doc )
    {
        MemoryInterfacePtr memory = Helper::loadStreamCacheArchiveMemory( _stream, m_archivator, _doc, __FILE__, __LINE__ );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, nullptr );

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DataflowPYZ::flow( const DataInterfacePtr & _data, const MemoryInterfacePtr & _memory, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        PythonScriptCodeData * data = stdex::intrusive_get<PythonScriptCodeData *>( _data );

        const uint8_t * source_buffer = _memory->getBuffer();
        size_t source_size = _memory->getSize();

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

        PyObject * py_code = m_kernel->marshal_get_object( (char *)source_buffer + 8, source_size - 8 );

        if( py_code == nullptr )
        {
            LOGGER_ERROR( "module invalid marshal get object"
            );

            return false;
        }

#ifdef MENGINE_DEBUG
        if( m_kernel->code_check( py_code ) == false )
        {
            LOGGER_ERROR( "module marshal get object not code"
            );

            return false;
        }
#endif

        data->setScriptCode( pybind::make_borrowed_t( m_kernel, py_code ) );

        return true;
    }
}