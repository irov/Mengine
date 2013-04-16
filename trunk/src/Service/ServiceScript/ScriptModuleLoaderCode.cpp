#	include "ScriptModuleLoaderCode.h"

#	include "Interface/StreamInterface.h"

#	include <pybind/../config/python.hpp>

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
    ScriptModuleLoaderCode::ScriptModuleLoaderCode( ServiceProviderInterface * _serviceProvider, InputStreamInterface * _stream, PyObject * _packagePath )
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
        if( m_stream )
        {
            m_stream->destroy();
        }

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
        PyObject * module = PyImport_AddModule( _module );

        PyObject * dict = PyModule_GetDict( module );

        PyDict_SetItemString( dict, "__loader__", m_embbed );

        if( m_packagePath != NULL )
        {
            PyDict_SetItemString( dict, "__path__", m_packagePath );
            pybind::decref( m_packagePath );
        }

        PyObject * code = this->unmarshal_code();

        if( code == NULL )
        {
            pybind::decref( module );

            return NULL;
        }

        module = PyImport_ExecCodeModule( const_cast<char *>(_module), code );

        pybind::decref( code );

        return module;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ScriptModuleLoaderCode::unmarshal_code()
    {
        size_t file_size = m_stream->size();
        
        if( file_size == 0 )
        {
            return NULL;
        }

        typedef std::vector<char> TVectorChar; 
        static TVectorChar buf;

        buf.resize( file_size );

        if( file_size > 0 )
        {
            m_stream->read( &buf[0], file_size );
        }

        m_stream->destroy();
        m_stream = NULL;
        
        long file_magic = get_int( (unsigned char *)&buf[0] );
        long py_magic = PyImport_GetMagicNumber();

        if( file_magic != py_magic )
        {
            return NULL;
        }

        PyObject * code = PyMarshal_ReadObjectFromString( &buf[0] + 8, file_size - 8 );

        if( code == NULL )
        {
            return NULL;
        }

        if( PyCode_Check(code) == 0 ) 
        {
            return NULL;
        }

        return code;
    }
}