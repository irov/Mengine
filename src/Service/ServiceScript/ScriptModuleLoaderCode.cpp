#   include "ScriptModuleLoaderCode.h"

#	include "Interface/ArchiveInterface.h"
#	include "Interface/MemoryInterface.h"
#	include "Interface/ScriptSystemInterface.h"

#	include "Core/MemoryHelper.h"

#   include "Logger/Logger.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    ScriptModuleLoaderCode::ScriptModuleLoaderCode()
    {
    }
    //////////////////////////////////////////////////////////////////////////
	PyObject * ScriptModuleLoaderCode::load_module( PyObject * _module )
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
     	
		PyObject * module = this->load_module_code_( _module, stream );

        return module;        
    }
    //////////////////////////////////////////////////////////////////////////
	PyObject * ScriptModuleLoaderCode::load_module_code_( PyObject * _module, const InputStreamInterfacePtr & _stream )
    {
		size_t file_size = _stream->size();

		if( file_size == 0 )
		{
			LOGGER_ERROR( m_serviceProvider )("ScriptModuleLoaderCode::unmarshal_code_ %s zero size"
				, pybind::string_to_char( _module )
				);

			return nullptr;
		}

		uint32_t code_size;
		_stream->read( &code_size, sizeof( code_size ) );

		uint32_t compress_size;
		_stream->read( &compress_size, sizeof( compress_size ) );

		MemoryInterfacePtr code_buffer = Helper::createMemoryCacheBuffer( m_serviceProvider, code_size, __FILE__, __LINE__ );

		if( code_buffer == nullptr )
		{
			return nullptr;
		}

		PyObject * py_module = SCRIPT_SERVICE( m_serviceProvider )
			->loadModuleBinary( _module, m_packagePath, code_buffer );

        return py_module;
    }
}