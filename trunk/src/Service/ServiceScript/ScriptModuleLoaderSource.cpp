#   include "ScriptModuleLoaderSource.h"

#   include "Interface/StringizeInterface.h"
#	include "Interface/ArchiveInterface.h"
#	include "Interface/CacheInterface.h"

#	include "Core/MemoryCacheBuffer.h"

#   include "Config/Blobject.h"

#   include "Logger/Logger.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    ScriptModuleLoaderSource::ScriptModuleLoaderSource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ScriptModuleLoaderSource::load_module( PyObject * _module )
    {   
		ConstStringHolderLocal holder_fullPath_local;
		ConstString c_fullPath_local;
		STRINGIZE_SERVICE(m_serviceProvider)
			->stringizeLocal( m_path.c_str(), m_path.size(), c_fullPath_local, holder_fullPath_local );

		InputStreamInterfacePtr stream = m_group->createInputFile( c_fullPath_local, false );

		if( stream == nullptr )
		{
			return nullptr;
		}

		if( m_group->openInputFile( c_fullPath_local, stream, 0, 0, false ) == false )
		{
			return nullptr;
		}
     	
		PyObject * module = this->load_module_source_( _module, stream );

        return module;        
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ScriptModuleLoaderSource::load_module_source_( PyObject * _module, const InputStreamInterfacePtr & _stream )
    {
        PyObject * code = this->unmarshal_source_( _module, _stream );

        if( code == nullptr )
        {
            return nullptr;
        }

		const char * str_module = pybind::string_to_char( _module );
        PyObject * py_module = pybind::module_init( str_module );

        PyObject * dict = pybind::module_dict( py_module );

        //pybind::dict_setstring( dict, "__loader__", m_embbed );

		if( m_packagePath == true )
		{
			PyObject * py_packagePath = pybind::build_value( "[O]", _module );
			pybind::dict_setstring( dict, "__path__", py_packagePath );
			pybind::decref( py_packagePath );
		}

        PyObject * py_module_exec = pybind::module_execcode( str_module, code );

        pybind::decref( code );

        return py_module_exec;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ScriptModuleLoaderSource::unmarshal_source_( PyObject * _module, const InputStreamInterfacePtr & _stream )
    {
        size_t file_size = _stream->size();

		MemoryCacheBufferPtr source_buffer = Helper::createMemoryBuffer( m_serviceProvider, file_size + 2, "unmarshal_source_" );

		if( source_buffer == nullptr )
		{
			return nullptr;
		}

		char * source_memory = source_buffer->getMemoryT<char *>();

        if( file_size > 0 )
        {
            _stream->read( source_memory, file_size );
        }

        source_memory[file_size] = '\n';
        source_memory[file_size + 1] = '\0';

		const char * str_module = pybind::string_to_char( _module );
        PyObject * code = pybind::code_compile_file( source_memory, str_module );

        if( code == nullptr )
        {
            pybind::check_error();

			LOGGER_ERROR(m_serviceProvider)("ScriptModuleLoaderSource::unmarshal_source_ %s invalid marshal get object"
				, pybind::string_to_char( _module )
				);

            return nullptr;
        }

        if( pybind::code_check( code ) == false ) 
        {
			LOGGER_ERROR(m_serviceProvider)("ScriptModuleLoaderSource::unmarshal_source_ %s marshal get object not code"
				, pybind::string_to_char( _module )
				);

            return nullptr;
        }

        return code;
    }
}