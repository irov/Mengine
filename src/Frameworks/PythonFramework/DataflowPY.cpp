#include "DataflowPY.h"

#include "Interface/ScriptServiceInterface.h"

#include "PythonScriptCodeData.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DataflowPY::DataflowPY()
        : m_kernel( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DataflowPY::~DataflowPY()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DataflowPY::setKernel( pybind::kernel_interface * _kernel )
    {
        m_kernel = _kernel;
    }
    //////////////////////////////////////////////////////////////////////////
    pybind::kernel_interface * DataflowPY::getKernel() const
    {
        return m_kernel;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DataflowPY::initialize()
    {
        m_factoryScriptCodeData = Helper::makeFactoryPool<PythonScriptCodeData, 128>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DataflowPY::finalize()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryScriptCodeData );
        m_factoryScriptCodeData = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DataflowPY::isThreadFlow() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    DataInterfacePtr DataflowPY::create( const DocumentPtr & _doc )
    {
        ScriptCodeDataPtr data = m_factoryScriptCodeData->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( data );

        return data;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr DataflowPY::load( const InputStreamInterfacePtr & _stream, const DocumentPtr & _doc )
    {
        MemoryInterfacePtr memory = Helper::createMemoryCacheStreamExtraSize( _stream, 2, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memory );

        Char * source_buffer = memory->getBuffer();

        MENGINE_ASSERTION_MEMORY_PANIC( source_buffer );

        size_t stream_size = _stream->size();

        source_buffer[stream_size + 0] = '\n';
        source_buffer[stream_size + 1] = '\0';

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DataflowPY::flow( const DataInterfacePtr & _data, const MemoryInterfacePtr & _memory, const DataflowContext * _context, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _doc );

        PythonScriptCodeData * data = stdex::intrusive_get<PythonScriptCodeData *>( _data );

        Char * source_buffer = _memory->getBuffer();

        PyObject * py_code = m_kernel->code_compile_file( source_buffer, _context->filePath.c_str() );

        MENGINE_ASSERTION_MEMORY_PANIC( py_code, "invalid marshal get object" );

        if( m_kernel->code_check( py_code ) == false )
        {
            LOGGER_ERROR( "marshal get object not code" );

            return false;
        }

        data->setScriptCode( pybind::make_borrowed_t( m_kernel, py_code ) );

        return true;
    }
}