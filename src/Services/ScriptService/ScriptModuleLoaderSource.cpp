#include "ScriptModuleLoaderSource.h"

#include "Interface/ArchivatorInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/ScriptServiceInterface.h"

#include "Kernel/MemoryHelper.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ScriptModuleLoaderSource::ScriptModuleLoaderSource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ScriptModuleLoaderSource::~ScriptModuleLoaderSource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ScriptModuleLoaderSource::load_module( pybind::kernel_interface * _kernel, PyObject * _module )
    {
        InputStreamInterfacePtr stream = m_group->createInputFile( m_path, false );

        if( stream == nullptr )
        {
            return nullptr;
        }

        if( m_group->openInputFile( m_path, stream, 0, 0, false ) == false )
        {
            return nullptr;
        }

        PyObject * module = this->load_module_source_( _kernel, _module, stream );

        return module;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ScriptModuleLoaderSource::load_module_source_( pybind::kernel_interface * _kernel, PyObject * _module, const InputStreamInterfacePtr & _stream )
    {
        (void)_kernel;

        size_t file_size = _stream->size();

        MemoryInterfacePtr source_buffer = Helper::createMemoryCacheBuffer( file_size + 2, "ScriptModuleLoaderSource", __FILE__, __LINE__ );

        if( source_buffer == nullptr )
        {
            return nullptr;
        }

        char * source_memory = source_buffer->getBuffer();

        if( file_size > 0 )
        {
            _stream->read( source_memory, file_size );
        }

        source_memory[file_size] = '\n';
        source_memory[file_size + 1] = '\0';

        PyObject * py_module = SCRIPT_SERVICE()
            ->loadModuleSource( _module, m_packagePath, source_buffer );

        return py_module;
    }
}