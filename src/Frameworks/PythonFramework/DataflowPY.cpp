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
    DataInterfacePtr DataflowPY::create( const DocumentInterfacePtr & _doc )
    {
        ScriptCodeDataPtr data = m_factoryScriptCodeData->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( data, "invalid create data" );

        return data;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr DataflowPY::load( const InputStreamInterfacePtr & _stream, const DocumentInterfacePtr & _doc )
    {
        MemoryInterfacePtr memory = Helper::createMemoryCacheStreamExtraSize( _stream, 2, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid create memory" );

        Char * source_buffer = memory->getBuffer();

        MENGINE_ASSERTION_MEMORY_PANIC( source_buffer, "invalid get memory buffer" );

        size_t stream_size = _stream->size();

        source_buffer[stream_size + 0] = '\n';
        source_buffer[stream_size + 1] = '\0';

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DataflowPY::flow( const DataInterfacePtr & _data, const MemoryInterfacePtr & _memory, const DataflowContext * _context, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _doc );

        PythonScriptCodeData * data = _data.getT<PythonScriptCodeData *>();

        Char * source_buffer = _memory->getBuffer();

        const Char * source_filepath = _context->filePath.c_str();

        PyObject * py_code = m_kernel->code_compile_file( source_buffer, source_filepath );

        if( py_code == nullptr )
        {
            LOGGER_ERROR( "invalid code compile file '%s'"
                , _context->filePath.c_str()
            );

            return false;
        }

        if( m_kernel->code_check( py_code ) == false )
        {
            LOGGER_ERROR( "marshal get object not code [file '%s']"
                , _context->filePath.c_str()
            );

            return false;
        }

        data->setScriptCode( pybind::make_borrowed_t( m_kernel, py_code ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}