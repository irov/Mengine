#   pragma once

#   include "Interface/StreamInterface.h"
#   include "Interface/FileSystemInterface.h"

#	include "Config/Typedef.h"
#	include "Config/String.h"

#   include "Core/ConstString.h"
#   include "Core/FilePath.h"

#   include "stdex/binary_vector.h"

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
        PyObject * find_module( const ConstString & _module, PyObject * _path );
        PyObject * load_module( const ConstString & _module );
        //PyObject * _find_and_load( PyObject * _module, PyObject * _builtins_import );

    protected:
        bool find_module_source_( const ConstString & _module );
        bool find_module_code_( const ConstString & _module );        

    protected:
        PyObject * load_module_source_( const ConstString & _module, const InputStreamInterfacePtr & _stream, PyObject * _packagePath );
        PyObject * load_module_code_( const ConstString & _module, const InputStreamInterfacePtr & _stream, PyObject * _packagePath );

    protected:
        PyObject * get_data( const InputStreamInterfacePtr & _stream );

        PyObject * unmarshal_source_( const ConstString & _module, const InputStreamInterfacePtr & _stream );
        PyObject * unmarshal_code_( const ConstString & _module, const InputStreamInterfacePtr & _stream );

    protected:
        bool convertDotToSlash_( char * _cache, size_t _cacheSize, const ConstString & _module, size_t & _modulePathCacheLen );

    protected:
        ServiceProviderInterface * m_serviceProvider;

        PyObject * m_embbed;
        
        struct ModulePathes
        {
            ConstString pak;
            TVectorFilePath pathes;
        };

        typedef std::vector<ModulePathes> TVectorModulePathes;
        TVectorModulePathes m_modulePaths;

        struct ModulePathCache
        {
            InputStreamInterfacePtr stream;

            PyObject * packagePath;
            bool source;
        };

        typedef stdex::binary_vector<ConstString, ModulePathCache> TMapModulePath;
        mutable TMapModulePath m_paths;

    protected:
        ModulePathCache * findModule_( const ConstString & _module, const char * _modulePath, size_t _modulePathLen ) const;
    };
}