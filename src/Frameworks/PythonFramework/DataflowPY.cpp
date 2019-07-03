#include "DataflowPY.h"

#include "Interface/ScriptServiceInterface.h"

#include "PythonScriptCodeData.h"

#include "Kernel/MemoryHelper.h"
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
        m_factoryScriptCodeData = new FactoryPool<PythonScriptCodeData, 128>();

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
    DataInterfacePtr DataflowPY::create( const Char * _doc )
    {
        ScriptCodeDataPtr data = m_factoryScriptCodeData->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( data, nullptr );

        return data;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr DataflowPY::load( const InputStreamInterfacePtr & _stream, const Char * _doc )
    {
        MemoryInterfacePtr memory = Helper::createMemoryCacheStreamExtraSize( _stream, 2, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, nullptr );

        Char * source_buffer = memory->getBuffer();

        MENGINE_ASSERTION_MEMORY_PANIC( source_buffer, nullptr );

        size_t stream_size = _stream->size();

        source_buffer[stream_size] = '\n';
        source_buffer[stream_size + 1] = '\0';

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DataflowPY::flow( const DataInterfacePtr & _data, const MemoryInterfacePtr & _memory, const Char * _doc )
    {
        PythonScriptCodeData * data = stdex::intrusive_get<PythonScriptCodeData *>( _data );

        Char * source_buffer = _memory->getBuffer();

        PyObject * py_code = m_kernel->code_compile_file( source_buffer, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( py_code, false, "invalid marshal get object" );

        if( m_kernel->code_check( py_code ) == false )
        {
            LOGGER_ERROR( "marshal get object not code"
            );

            return false;
        }

        data->setScriptCode( pybind::make_borrowed_t( m_kernel, py_code ) );
        
        return true;
    }
}