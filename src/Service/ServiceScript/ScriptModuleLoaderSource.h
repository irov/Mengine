#   pragma once

#	include "Config/Typedef.h"
#   include "Core/ConstString.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
    class ServiceProviderInterface;
    class InputStreamInterface;

    class ScriptModuleLoaderSource
    {
    public:
        ScriptModuleLoaderSource( ServiceProviderInterface * _serviceProvider, InputStreamInterface * _stream, PyObject * _packagePath );
        ~ScriptModuleLoaderSource();

    public:
        PyObject * load_module( const char * _module );

    protected:
        PyObject * unmarshal_code( const char * _module );

    public:
        void setEmbbed( PyObject * _embbed );

    protected:
        ServiceProviderInterface * m_serviceProvider;
        
        InputStreamInterface * m_stream;
        PyObject * m_packagePath;

        PyObject * m_embbed;
    };
}