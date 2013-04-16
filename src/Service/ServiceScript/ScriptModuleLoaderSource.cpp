#	include "ScriptModuleLoaderSource.h"

#	include "Interface/StreamInterface.h"

#	include <pybind/../config/python.hpp>

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    ScriptModuleLoaderSource::ScriptModuleLoaderSource( ServiceProviderInterface * _serviceProvider, InputStreamInterface * _stream, PyObject * _packagePath )
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
    ScriptModuleLoaderSource::~ScriptModuleLoaderSource()
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
    void ScriptModuleLoaderSource::setEmbbed( PyObject * _embbed )
    {
        m_embbed = _embbed;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ScriptModuleLoaderSource::load_module( const char * _module )
    {
        PyObject * module = PyImport_AddModule( _module );

        PyObject * dict = PyModule_GetDict( module );

        PyDict_SetItemString( dict, "__loader__", m_embbed );

        if( m_packagePath != NULL )
        {
            PyDict_SetItemString( dict, "__path__", m_packagePath );
            pybind::decref( m_packagePath );
        }

        PyObject * code = this->unmarshal_code( _module );

        if( code == NULL )
        {
            pybind::decref( module );

            return NULL;
        }

        pybind::incref( module );
        module = PyImport_ExecCodeModule( const_cast<char *>(_module), code );
        pybind::decref( module );

        pybind::decref( code );

        return module;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ScriptModuleLoaderSource::unmarshal_code( const char * _module )
    {
        size_t file_size = m_stream->size();

        typedef std::vector<char> TVectorChar; 
        static TVectorChar buf;

        buf.resize( file_size + 2 );

        if( file_size > 0 )
        {
            m_stream->read( &buf[0], file_size );
        }

        buf[file_size] = '\n';
        buf[file_size + 1] = '\0';

        m_stream->destroy();
        m_stream = NULL;

        PyObject * code = Py_CompileString( &buf[0], _module, Py_file_input );
        
        if( code == NULL )
        {
            if( PyErr_Occurred() )
            {
                PyErr_Print();
            }

            return NULL;
        }

        if( PyCode_Check(code) == 0 ) 
        {
            return NULL;
        }

        return code;
    }
}