#	include "ScriptModuleLoaderCode.h"

#	include "Interface/StreamInterface.h"
#	include "Interface/ArchiveInterface.h"

#   include "Logger/Logger.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    static int get_int( unsigned char * _buff )
    {
        int x;
        x =  (int)_buff[0];
        x |= (int)_buff[1] << 8;
        x |= (int)_buff[2] << 16;
        x |= (int)_buff[3] << 24;

        return x;
    }
    //////////////////////////////////////////////////////////////////////////
    ScriptModuleLoaderCode::ScriptModuleLoaderCode( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, PyObject * _packagePath )
        : m_serviceProvider(_serviceProvider)
        , m_stream(_stream)
        , m_packagePath(_packagePath)
        , m_embbed(NULL)
    {
        if( m_packagePath )
        {
            pybind::incref( m_packagePath );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    ScriptModuleLoaderCode::~ScriptModuleLoaderCode()
    {
        if( m_packagePath )
        {
            pybind::decref( m_packagePath );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ScriptModuleLoaderCode::setEmbbed( PyObject * _embbed )
    {
        m_embbed = _embbed;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ScriptModuleLoaderCode::load_module( const char * _module )
    {   
        PyObject * module = pybind::module_init( _module );

        PyObject * dict = pybind::module_dict( module );

        pybind::dict_set( dict, "__loader__", m_embbed );

        if( m_packagePath != NULL )
        {
            pybind::dict_set( dict, "__path__", m_packagePath );
            pybind::decref( m_packagePath );
        }

        PyObject * code = this->unmarshal_code();

        if( code == NULL )
        {
            pybind::decref( module );

            return NULL;
        }

        module = pybind::module_execcode( _module, code );

        pybind::decref( code );

        return module;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ScriptModuleLoaderCode::unmarshal_code()
    {
        size_t file_size = m_stream->size();
        
        if( file_size == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("ScriptModuleLoaderCode::unmarshal_code zero size"
                );

            return NULL;
        }

        size_t code_size;
        m_stream->read( &code_size, sizeof(code_size) );

        size_t compress_size;
        m_stream->read( &compress_size, sizeof(compress_size) );

        static TBlobject compress_buf;

        compress_buf.resize( compress_size );

        m_stream->read( &compress_buf[0], compress_size );

        static TBlobject code_buf;

        code_buf.resize( code_size );
        
        size_t uncompress_size;
        if( ARCHIVE_SERVICE(m_serviceProvider)
            ->uncompress( &code_buf[0], code_size, uncompress_size, &compress_buf[0], compress_size ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("ScriptModuleLoaderCode::unmarshal_code uncompress failed"
                );

            return NULL;
        }

        //buf.resize( file_size );

        //if( file_size > 0 )
        //{
        //    m_stream->read( &buf[0], file_size );
        //}

        m_stream = nullptr;
        
        long file_magic = get_int( &code_buf[0] );
        long py_magic = pybind::marshal_magic_number();

        if( file_magic != py_magic )
        {
            return NULL;
        }

        PyObject * code = pybind::marshal_get_object( (char *)&code_buf[0] + 8, code_size - 8 );

        if( code == NULL )
        {
            return NULL;
        }

        if( pybind::code_check( code ) == false ) 
        {
            return NULL;
        }

        return code;
    }
}