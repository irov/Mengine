#   include "ScriptModuleLoaderCode.h"

#   include "Interface/StringizeInterface.h"
#	include "Interface/ArchiveInterface.h"
#	include "Interface/MemoryInterface.h"

#	include "Core/MemoryHelper.h"

#   include "Logger/Logger.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    static int s_get_int( const uint8_t * _buff )
    {
        int x;
        x =  (int)_buff[0];
        x |= (int)_buff[1] << 8;
        x |= (int)_buff[2] << 16;
        x |= (int)_buff[3] << 24;

        return x;
    }
    //////////////////////////////////////////////////////////////////////////
    ScriptModuleLoaderCode::ScriptModuleLoaderCode()
    {
    }
    //////////////////////////////////////////////////////////////////////////
	PyObject * ScriptModuleLoaderCode::load_module( pybind::kernel_interface * _kernel, PyObject * _module )
    {   
        ConstString c_fullPath = Helper::stringizeString( m_serviceProvider, m_path );

		InputStreamInterfacePtr stream = m_group->createInputFile( c_fullPath, false );

		if( stream == nullptr )
		{
			return nullptr;
		}

		if( m_group->openInputFile( c_fullPath, stream, 0, 0, false ) == false )
		{
			return nullptr;
		}
     	
		PyObject * module = this->load_module_code_( _kernel, _module, stream );

        return module;        
    }
    //////////////////////////////////////////////////////////////////////////
	PyObject * ScriptModuleLoaderCode::load_module_code_( pybind::kernel_interface * _kernel, PyObject * _module, const InputStreamInterfacePtr & _stream )
    {
        PyObject * code = this->unmarshal_code_( _kernel, _module, _stream );

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
			pybind::dict_setstring_t( _kernel, dict, "__path__", py_packagePath );
        }

        PyObject * py_module_exec = pybind::module_execcode( str_module, code );

        pybind::decref( code );

        return py_module_exec;
    }
    //////////////////////////////////////////////////////////////////////////
	PyObject * ScriptModuleLoaderCode::unmarshal_code_( pybind::kernel_interface * _kernel, PyObject * _module, const InputStreamInterfacePtr & _stream )
    {
		(void)_kernel;

        size_t file_size = _stream->size();
				
        if( file_size == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("ScriptModuleLoaderCode::unmarshal_code_ %s zero size"
                , pybind::string_to_char( _module )
                );

            return nullptr;
        }

        uint32_t code_size;
        _stream->read( &code_size, sizeof(code_size) );

        uint32_t compress_size;
        _stream->read( &compress_size, sizeof(compress_size) );
		
		MemoryInterfacePtr code_buffer = Helper::createMemoryCacheBuffer( m_serviceProvider, code_size, __FILE__, __LINE__ );

		if( code_buffer == nullptr )
		{
			return nullptr;
		}

		uint8_t * code_memory = code_buffer->getMemory();

        size_t uncompress_size;
        if( ARCHIVE_SERVICE(m_serviceProvider)
			->decompressStream( m_archivator, _stream, compress_size, code_memory, code_size, uncompress_size ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("ScriptModuleLoaderCode::unmarshal_code_ %s uncompress failed"
				, pybind::string_to_char( _module )
                );

            return nullptr;
        }
                
        long file_magic = s_get_int( code_memory );
        long py_magic = pybind::marshal_magic_number();

        if( file_magic != py_magic )
        {
			LOGGER_ERROR(m_serviceProvider)("ScriptModuleLoaderCode::unmarshal_code_ %s invalid magic %u need %u"
				, pybind::string_to_char( _module )
				, file_magic
				, py_magic
				);

            return nullptr;
        }

        PyObject * code = pybind::marshal_get_object( (char *)code_memory + 8, uncompress_size - 8 );

        if( code == nullptr )
        {
			pybind::check_error();

			LOGGER_ERROR(m_serviceProvider)("ScriptModuleLoaderCode::unmarshal_code_ %s invalid marshal get object"
				, pybind::string_to_char( _module )
				);

            return nullptr;
        }

        if( pybind::code_check( code ) == false ) 
        {
			LOGGER_ERROR(m_serviceProvider)("ScriptModuleLoaderCode::unmarshal_code_ %s marshal get object not code"
				, pybind::string_to_char( _module )
				);

            return nullptr;
        }

        return code;
    }
}