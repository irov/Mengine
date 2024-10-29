#include "DataflowPYZ.h"

#include "Interface/ScriptServiceInterface.h"

#include "PythonScriptCodeData.h"

#include "Kernel/StreamHelper.h"
#include "Kernel/MemoryStreamHelper.h"
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
        m_factoryScriptCodeData = Helper::makeFactoryPool<PythonScriptCodeData, 128>( MENGINE_DOCUMENT_FACTORABLE );

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
    DataInterfacePtr DataflowPYZ::create( const DocumentInterfacePtr & _doc )
    {
        ScriptCodeDataPtr data = m_factoryScriptCodeData->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( data, "invalid create data" );

        return data;
    }
    //////////////////////////////////////////////////////////////////////////
    static int32_t s_get_int( const uint8_t * _buff )
    {
        int32_t x = MENGINE_FOURCC( (int32_t)_buff[0], (int32_t)_buff[1], (int32_t)_buff[2], (int32_t)_buff[3] );

        return x;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr DataflowPYZ::load( const InputStreamInterfacePtr & _stream, const DocumentInterfacePtr & _doc )
    {
        MemoryInterfacePtr memory = Helper::readStreamCacheArchiveMemory( _stream, m_archivator, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid read stream cache archive memory" );

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DataflowPYZ::flow( const DataInterfacePtr & _data, const MemoryInterfacePtr & _memory, const DataflowContext * _context, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _doc );

        PythonScriptCodeData * data = _data.getT<PythonScriptCodeData *>();

        const uint8_t * source_buffer = _memory->getBuffer();
        size_t source_size = _memory->getSize();

        MENGINE_ASSERTION_MEMORY_PANIC( source_buffer, "module invalid buffer [doc: %s]"
            , MENGINE_DOCUMENT_STR( _doc )
        );

        long file_magic = s_get_int( source_buffer );
        long py_magic = m_kernel->marshal_magic_number();

        if( file_magic != py_magic )
        {
            LOGGER_ERROR( "module invalid magic '%ld' need '%ld'"
                , file_magic
                , py_magic
            );

            return false;
        }

        PyObject * py_code = m_kernel->marshal_get_object( (char *)source_buffer + 8, source_size - 8 );

        MENGINE_ASSERTION_MEMORY_PANIC( py_code, "module invalid marshal get object [doc: %s]"
            , MENGINE_DOCUMENT_STR( _doc )
        );

#if defined(MENGINE_DEBUG)
        if( m_kernel->code_check( py_code ) == false )
        {
            LOGGER_ERROR( "module marshal get object not code" );

            return false;
        }
#endif

        data->setScriptCode( pybind::make_borrowed_t( m_kernel, py_code ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}