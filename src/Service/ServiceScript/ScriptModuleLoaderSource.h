#   pragma once

#   include "Interface/StreamInterface.h"

#	include "Config/Typedef.h"
#   include "Core/ConstString.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
    class ServiceProviderInterface;    

    class ScriptModuleLoaderSource
    {
    public:
        ScriptModuleLoaderSource( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, PyObject * _packagePath );
        ~ScriptModuleLoaderSource();

    public:
        PyObject * load_module( const char * _module );

    protected:
        PyObject * unmarshal_code( const char * _module );

    public:
        void setEmbbed( PyObject * _embbed );

    protected:
        ServiceProviderInterface * m_serviceProvider;
        
        InputStreamInterfacePtr m_stream;
        PyObject * m_packagePath;

        PyObject * m_embbed;
    };
}