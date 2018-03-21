#include "ScriptModuleLoaderCode.h"

#include "Interface/ArchiveInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/ScriptSystemInterface.h"

#include "Core/MemoryHelper.h"

#include "Logger/Logger.h"

namespace Mengine
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
			LOGGER_ERROR("ScriptModuleLoaderCode::unmarshal_code_ %s zero size"
				, pybind::string_to_char( _module )
				);

			return nullptr;
		}

		uint32_t code_size;
		_stream->read( &code_size, sizeof( code_size ) );

		uint32_t compress_size;
		_stream->read( &compress_size, sizeof( compress_size ) );

		MemoryInterfacePtr code_buffer = Helper::createMemoryCacheBuffer( code_size, __FILE__, __LINE__ );

		if( code_buffer == nullptr )
		{
			return nullptr;
		}

		uint8_t * code_memory = code_buffer->getMemory();

		size_t uncompress_size;
		if( ARCHIVE_SERVICE()
			->decompressStream( m_archivator, _stream, compress_size, code_memory, code_size, uncompress_size ) == false )
		{
			LOGGER_ERROR("ScriptModuleLoaderCode::unmarshal_code_ %s uncompress failed"
				, pybind::string_to_char( _module )
				);

			return nullptr;
		}

		PyObject * py_module = SCRIPT_SERVICE()
			->loadModuleBinary( _module, m_packagePath, code_buffer );

        return py_module;
    }
}