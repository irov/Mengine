#include "ScriptModuleFinder.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/ArchiveInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/ArchiveServiceInterface.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"

#include "Kernel/Logger.h"

#include <algorithm>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ScriptModuleFinder::ScriptModuleFinder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ScriptModuleFinder::~ScriptModuleFinder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ScriptModuleFinder::initialize()
    {
        const ArchivatorInterfacePtr & archivator = ARCHIVE_SERVICE()
            ->getArchivator( STRINGIZE_STRING_LOCAL( "zip" ) );

        if( archivator == nullptr )
        {
            return false;
        }

        m_archivator = archivator;

        m_factoryScriptModuleLoaderCode = new FactoryPool<ScriptModuleLoaderCode, 8>();

#ifndef MENGINE_MASTER_RELEASE
        m_factoryScriptModuleLoaderSource = new FactoryPool<ScriptModuleLoaderSource, 8>();
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ScriptModuleFinder::finalize()
    {
        m_embed = nullptr;

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryScriptModuleLoaderCode );

        m_factoryScriptModuleLoaderCode = nullptr;

#ifndef MENGINE_MASTER_RELEASE
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryScriptModuleLoaderSource );

        m_factoryScriptModuleLoaderSource = nullptr;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void ScriptModuleFinder::setEmbed( const pybind::object & _embed )
    {
        m_embed = _embed;
    }
    //////////////////////////////////////////////////////////////////////////
    void ScriptModuleFinder::addModulePath( const ConstString & _pack, const VectorConstString & _pathes )
    {
        ModulePathes mp;
        mp.pack = _pack;
        mp.pathes = _pathes;

        m_modulePaths.emplace_back( mp );
    }
    //////////////////////////////////////////////////////////////////////////
    namespace
    {
        class FModuleRemove
        {
        public:
            FModuleRemove( const ConstString & _pack )
                : m_pack( _pack )
            {
            }

        protected:
            void operator = ( const FModuleRemove & )
            {
            }

        public:
            bool operator() ( const ModulePathes & _path )
            {
                if( _path.pack != m_pack )
                {
                    return false;
                }

                return true;
            }

        protected:
            const ConstString & m_pack;
        };
    }
    //////////////////////////////////////////////////////////////////////////
    void ScriptModuleFinder::removeModulePath( const ConstString & _pack )
    {
        m_modulePaths.erase(
            std::find_if( m_modulePaths.begin(), m_modulePaths.end(), FModuleRemove( _pack ) )
            , m_modulePaths.end()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ScriptModuleFinder::find_module( pybind::kernel_interface * _kernel, PyObject * _module, PyObject * _path )
    {
        (void)_module;
        (void)_path;

#ifndef MENGINE_MASTER_RELEASE
        {
            ScriptModuleLoaderPtr loaderSource = m_factoryScriptModuleLoaderSource->createObject();

            loaderSource->setModule( _kernel, _module );

            if( this->find_module_source_( _kernel, _module, loaderSource ) == true )
            {
                m_loaders.emplace_back( loaderSource );

                return m_embed.ret();
            }
        }
#endif

        {
            ScriptModuleLoaderPtr loaderCode = m_factoryScriptModuleLoaderCode->createObject();

            loaderCode->setModule( _kernel, _module );

            if( this->find_module_code_( _kernel, _module, loaderCode ) == true )
            {
                m_loaders.emplace_back( loaderCode );

                return m_embed.ret();
            }
        }

        return _kernel->ret_none();
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ScriptModuleFinder::load_module( pybind::kernel_interface * _kernel, PyObject * _module )
    {
        ScriptModuleLoaderPtr loader = m_loaders.back();
        m_loaders.pop_back();

        PyObject * loader_module = loader->getModule();

        if( loader_module != _module && _kernel->test_equal( loader_module, _module ) == false )
        {
            return nullptr;
        }

        PyObject * py_code = loader->load_module( _kernel, _module );

        return py_code;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ScriptModuleFinder::convertDotToSlash_( pybind::kernel_interface * _kernel, Char * _cache, uint32_t _cacheSize, PyObject * _module, uint32_t & _modulePathCacheLen )
    {
        uint32_t module_size;
        const char * module_str = _kernel->string_to_char_and_size( _module, module_size );

        if( stdex::memorycopy_safe( _cache, 0, _cacheSize, module_str, module_size ) == false )
        {
            return false;
        }

        _cache[module_size] = '\0';

        for( char
            *it = _cache,
            *it_end = _cache + module_size;
            it != it_end;
            ++it )
        {
            if( *it == '.' )
            {
                *it = '/';
            }
        }

        _modulePathCacheLen = module_size;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ScriptModuleFinder::find_module_source_( pybind::kernel_interface * _kernel, PyObject * _module, const ScriptModuleLoaderPtr & _loader )
    {
        bool successful = this->find_module_( _kernel, _module, _loader, ".py", sizeof( ".py" ) - 1, "__init__.py", sizeof( "__init__.py" ) - 1 );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ScriptModuleFinder::find_module_code_( pybind::kernel_interface * _kernel, PyObject * _module, const ScriptModuleLoaderPtr &  _loader )
    {
        bool successful = this->find_module_( _kernel, _module, _loader, ".pyz", sizeof( ".pyz" ) - 1, "__init__.pyz", sizeof( "__init__.pyz" ) - 1 );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ScriptModuleFinder::find_module_( pybind::kernel_interface * _kernel, PyObject * _module, const ScriptModuleLoaderPtr & _loader, const Char * _ext, uint32_t _extN, const char * _init, uint32_t _extI )
    {
        Char modulePathCache[MENGINE_MAX_PATH];

        uint32_t modulePathCacheLen;
        if( this->convertDotToSlash_( _kernel, modulePathCache, MENGINE_MAX_PATH, _module, modulePathCacheLen ) == false )
        {
            return false;
        }

        if( stdex::memorycopy_safe( modulePathCache, modulePathCacheLen, MENGINE_MAX_PATH, _ext, _extN ) == false )
        {
            return false;
        }

        if( this->findModule_( modulePathCache, modulePathCacheLen + _extN, _loader ) == false )
        {
            modulePathCache[modulePathCacheLen] = '/';
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
    bool ScriptModuleFinder::findModule_( const Char * _modulePath, uint32_t _modulePathLen, const ScriptModuleLoaderPtr & _loader ) const
    {
        for( const ModulePathes & mp : m_modulePaths )
        {
            FileGroupInterfacePtr fileGroup = FILE_SERVICE()
                ->getFileGroup( mp.pack );

            if( fileGroup == nullptr )
            {
                continue;
            }

            for( const ConstString & path : mp.pathes )
            {
                m_cacheFullPath.clear();
                m_cacheFullPath += path;
                m_cacheFullPath.append( _modulePath, _modulePathLen );

                FilePath c_fullPath = Helper::stringizeFilePath( m_cacheFullPath );

                if( fileGroup->existFile( c_fullPath ) == false )
                {
                    continue;
                }

                _loader->initialize( fileGroup, c_fullPath, m_archivator );

                return true;
            }
        }

        return false;
    }
}
