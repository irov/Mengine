#	include "ScriptModuleLoaderSource.h"

#	include "Interface/StreamInterface.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    ScriptModuleLoaderSource::ScriptModuleLoaderSource( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, PyObject * _packagePath )
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
        PyObject * module = pybind::module_init( _module );

        PyObject * dict = pybind::module_dict( module );

        pybind::dict_set( dict, "__loader__", m_embbed );

        if( m_packagePath != NULL )
        {
            pybind::dict_set( dict, "__path__", m_packagePath );
            pybind::decref( m_packagePath );
        }

        PyObject * code = this->unmarshal_code( _module );

        if( code == NULL )
        {
            pybind::decref( module );

            return NULL;
        }

        pybind::incref( module );
        module = pybind::module_execcode( _module, code );
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

        m_stream = nullptr;

        PyObject * code = pybind::code_compile_file( &buf[0], _module );
        
        if( code == NULL )
        {
            pybind::check_error();

            return NULL;
        }

        if( pybind::code_check( code ) == false ) 
        {
            return NULL;
        }

        return code;
    }
}