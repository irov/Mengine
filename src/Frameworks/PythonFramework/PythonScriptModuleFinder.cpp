#include "PythonScriptModuleFinder.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/ArchiveServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/FactoryWithMutex.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/FilePathHelper.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"

#include "Config/Algorithm.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonScriptModuleFinder::PythonScriptModuleFinder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonScriptModuleFinder::~PythonScriptModuleFinder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptModuleFinder::setDataflowPY( const DataflowInterfacePtr & _dataflowPY )
    {
        m_dataflowPY = _dataflowPY;
    }
    //////////////////////////////////////////////////////////////////////////
    const DataflowInterfacePtr & PythonScriptModuleFinder::getDataflowPY() const
    {
        return m_dataflowPY;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptModuleFinder::setDataflowPYZ( const DataflowInterfacePtr & _dataflowPYZ )
    {
        m_dataflowPYZ = _dataflowPYZ;
    }
    //////////////////////////////////////////////////////////////////////////
    const DataflowInterfacePtr & PythonScriptModuleFinder::getDataflowPYZ() const
    {
        return m_dataflowPYZ;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonScriptModuleFinder::initialize()
    {
        FactoryWithMutexPtr factory = Helper::makeFactoryPool<PythonScriptModuleLoader, 8, FactoryWithMutex>( MENGINE_DOCUMENT_FACTORABLE );

        ThreadMutexInterfacePtr mutex = THREAD_SERVICE()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        factory->setMutex( mutex );

        m_factoryScriptModuleLoader = factory;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptModuleFinder::finalize()
    {
        m_dataflowPY = nullptr;
        m_dataflowPYZ = nullptr;

        m_embed = nullptr;

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryScriptModuleLoader );

        m_factoryScriptModuleLoader = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptModuleFinder::setEmbed( const pybind::object & _embed )
    {
        m_embed = _embed;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptModuleFinder::addModulePath( const FileGroupInterfacePtr & _fileGroup, const VectorFilePath & _pathes )
    {
        ModulePathes mp;
        mp.fileGroup = _fileGroup;
        mp.pathes = _pathes;

        m_modulePaths.emplace_back( mp );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptModuleFinder::removeModulePath( const FileGroupInterfacePtr & _fileGroup )
    {
        m_modulePaths.erase(
            Algorithm::find_if( m_modulePaths.begin(), m_modulePaths.end(), [&_fileGroup]( const ModulePathes & _pathes )
        {
            return _pathes.fileGroup == _fileGroup;
        } )
            , m_modulePaths.end()
            );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * PythonScriptModuleFinder::find_module( pybind::kernel_interface * _kernel, PyObject * _module, PyObject * _path )
    {
        MENGINE_UNUSED( _path );

#ifndef MENGINE_MASTER_RELEASE
        {
            ScriptModuleLoaderPtr loader = m_factoryScriptModuleLoader->createObject( MENGINE_DOCUMENT_FACTORABLE );

            loader->setDataflow( m_dataflowPY );
            loader->setModule( _kernel, _module );

            if( this->find_module_source_( _kernel, _module, loader ) == true )
            {
                m_loaders.emplace_back( loader );

                return m_embed.ret();
            }

            loader->finalize();
        }
#endif

        {
            ScriptModuleLoaderPtr loader = m_factoryScriptModuleLoader->createObject( MENGINE_DOCUMENT_FACTORABLE );

            loader->setDataflow( m_dataflowPYZ );
            loader->setModule( _kernel, _module );

            if( this->find_module_code_( _kernel, _module, loader ) == true )
            {
                m_loaders.emplace_back( loader );

                return m_embed.ret();
            }

            loader->finalize();
        }

        return _kernel->ret_none();
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * PythonScriptModuleFinder::load_module( pybind::kernel_interface * _kernel, PyObject * _module )
    {
        ScriptModuleLoaderPtr loader = m_loaders.back();
        m_loaders.pop_back();

        PyObject * loader_module = loader->getModule();

        if( loader_module != _module && _kernel->test_equal( loader_module, _module ) == false )
        {
            return nullptr;
        }

        PyObject * py_code = loader->load_module( _kernel, _module );

        loader->finalize();

        return py_code;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonScriptModuleFinder::convertDotToSlash_( pybind::kernel_interface * _kernel, Char * const _cache, size_t _cacheSize, PyObject * _module, size_t * const _modulePathCacheLen )
    {
        size_t module_size;
        const char * module_str = _kernel->string_to_char_and_size( _module, &module_size );

        if( stdex::memorycopy_safe( _cache, 0, _cacheSize, module_str, module_size ) == false )
        {
            return false;
        }

        _cache[module_size] = '\0';

        for( char
            * it = _cache,
            *it_end = _cache + module_size;
            it != it_end;
            ++it )
        {
            if( *it == '.' )
            {
                *it = MENGINE_PATH_DELIM;
            }
        }

        *_modulePathCacheLen = module_size;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonScriptModuleFinder::find_module_source_( pybind::kernel_interface * _kernel, PyObject * _module, const ScriptModuleLoaderPtr & _loader )
    {
        bool successful = this->find_module_( _kernel, _module, _loader, ".py", sizeof( ".py" ) - 1, "__init__.py", sizeof( "__init__.py" ) - 1 );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonScriptModuleFinder::find_module_code_( pybind::kernel_interface * _kernel, PyObject * _module, const ScriptModuleLoaderPtr & _loader )
    {
        bool successful = this->find_module_( _kernel, _module, _loader, ".pyz", sizeof( ".pyz" ) - 1, "__init__.pyz", sizeof( "__init__.pyz" ) - 1 );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonScriptModuleFinder::find_module_( pybind::kernel_interface * _kernel, PyObject * _module, const ScriptModuleLoaderPtr & _loader, const Char * _ext, size_t _extN, const Char * _init, size_t _extI )
    {
        Char modulePathCache[MENGINE_MAX_PATH] = {'\0'};

        size_t modulePathCacheLen;
        if( this->convertDotToSlash_( _kernel, modulePathCache, MENGINE_MAX_PATH, _module, &modulePathCacheLen ) == false )
        {
            return false;
        }

        if( stdex::memorycopy_safe( modulePathCache, modulePathCacheLen, MENGINE_MAX_PATH, _ext, _extN ) == false )
        {
            return false;
        }

        if( this->findModule_( modulePathCache, modulePathCacheLen + _extN, _loader ) == false )
        {
            modulePathCache[modulePathCacheLen] = MENGINE_PATH_DELIM;
            if( stdex::memorycopy_safe( modulePathCache, modulePathCacheLen + 1, MENGINE_MAX_PATH, _init, _extI ) == false )
            {
                return false;
            }

            if( this->findModule_( modulePathCache, modulePathCacheLen + 1 + _extI, _loader ) == false )
            {
                return false;
            }

            _loader->setPackagePath( true );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonScriptModuleFinder::findModule_( const Char * _modulePath, size_t _modulePathLen, const ScriptModuleLoaderPtr & _loader ) const
    {
        for( const ModulePathes & mp : m_modulePaths )
        {
            const FileGroupInterfacePtr & fileGroup = mp.fileGroup;

            if( fileGroup == nullptr )
            {
                continue;
            }

            for( const FilePath & path : mp.pathes )
            {
                m_cacheFullPath = path;
                m_cacheFullPath.append( _modulePath, (PathString::size_type)_modulePathLen );

                FilePath c_fullPath = Helper::stringizeFilePath( m_cacheFullPath );

                if( fileGroup->existFile( c_fullPath, true ) == false )
                {
                    continue;
                }

                _loader->initialize( fileGroup, c_fullPath );

                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}
