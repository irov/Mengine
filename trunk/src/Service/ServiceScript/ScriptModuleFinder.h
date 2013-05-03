#   pragma once

#   include "Interface/StreamInterface.h"

#	include "Config/Typedef.h"

#   include "Core/ConstString.h"
#   include "Core/FilePath.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
    class ServiceProviderInterface;    

    class ScriptModuleFinder
    {
    public:
        ScriptModuleFinder( ServiceProviderInterface * _serviceProvider );

    public:
        void addModulePath( const ConstString & _pak, const TVectorFilePath & _pathes );

    public:
        void setEmbbed( PyObject * _embbed );

    public:
        PyObject * find_module( const char * _module, PyObject * _path );
        PyObject * _find_and_load( PyObject * _module, PyObject * _builtins_import );

    protected:
        PyObject * find_module_source_( const char * _module );
        PyObject * find_module_code_( const char * _module );        

    protected:
        PyObject * get_data( const InputStreamInterfacePtr & _stream );
        PyObject * unmarshal_code( PyObject * data );

    protected:
        InputStreamInterfacePtr findModule( const String & _modulePath ) const;

    protected:
        void convertDotToSlash_( const char * _module );
 
    protected:
        ServiceProviderInterface * m_serviceProvider;
        
        struct ModulePathes
        {
            ConstString pak;
            TVectorFilePath pathes;
        };

        typedef std::vector<ModulePathes> TVectorModulePathes;
        TVectorModulePathes m_modulePaths;

        mutable String m_modulePathCache;
        mutable String m_fullModulePathCache;
    };
}