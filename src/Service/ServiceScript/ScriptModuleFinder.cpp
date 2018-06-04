#include "ScriptModuleFinder.h"

#include "Interface/StringizeInterface.h"
#include "Interface/ArchiveInterface.h"
#include "Interface/MemoryInterface.h"

#include "Factory/FactoryPool.h"

#include "Logger/Logger.h"

#include <algorithm>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ScriptModuleFinder::ScriptModuleFinder()
        : m_embed(nullptr)
    {
    }
	//////////////////////////////////////////////////////////////////////////
	bool ScriptModuleFinder::initialize()
	{
        const ArchivatorInterfacePtr & archivator = ARCHIVE_SERVICE()
			->getArchivator( STRINGIZE_STRING_LOCAL( "zip") );

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
		pybind::decref( m_embed );
		m_embed = nullptr;

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryScriptModuleLoaderCode );

        m_factoryScriptModuleLoaderCode = nullptr;

#   ifndef MENGINE_MASTER_RELEASE
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryScriptModuleLoaderSource );

        m_factoryScriptModuleLoaderSource = nullptr;
#   endif
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptModuleFinder::setEmbed( PyObject * _embed )
	{
		pybind::decref( m_embed );
		m_embed = _embed;
		pybind::incref( m_embed );
	}
    //////////////////////////////////////////////////////////////////////////
	void ScriptModuleFinder::addModulePath( const ConstString & _pack, const TVectorConstString & _pathes )
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
			void operator = (const FModuleRemove &)
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
	PyObject * ScriptModuleFinder::find_module( PyObject * _module, PyObject * _path )
    {
        (void)_module;
        (void)_path;

#   ifndef MENGINE_MASTER_RELEASE
		{
			ScriptModuleLoaderPtr loaderSource = m_factoryScriptModuleLoaderSource->createObject();

			loaderSource->setModule( _module );

			if( this->find_module_source_( _module, loaderSource ) == true )
			{
				m_loaders.emplace_back( loaderSource );

				pybind::incref( m_embed );

				return m_embed;
			}
		}
#   endif
	
		{
			ScriptModuleLoaderPtr loaderCode = m_factoryScriptModuleLoaderCode->createObject();

			loaderCode->setModule( _module );

			if( this->find_module_code_( _module, loaderCode ) == true )
			{
				m_loaders.emplace_back( loaderCode );

				pybind::incref( m_embed );

				return m_embed;
			}
		}

		return pybind::ret_none();
    }
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptModuleFinder::load_module( PyObject * _module )
	{
		ScriptModuleLoaderPtr loader = m_loaders.back();
		m_loaders.pop_back();

		PyObject * loader_module = loader->getModule();

		if( loader_module != _module && pybind::test_equal( loader_module, _module ) == false )
		{			
			return nullptr;
		}

		PyObject * py_code = loader->load_module( _module );

		return py_code;
	}
    //////////////////////////////////////////////////////////////////////////
    bool ScriptModuleFinder::convertDotToSlash_( char * _cache, size_t _cacheSize, PyObject * _module, size_t & _modulePathCacheLen )
    {
		size_t module_size;
		const char * module_str = pybind::string_to_char_and_size( _module, module_size );
		
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
	bool ScriptModuleFinder::find_module_source_( PyObject * _module, const ScriptModuleLoaderPtr & _loader )
	{
		bool successful = this->find_module_( _module, _loader, ".py", sizeof(".py") - 1, "__init__.py", sizeof("__init__.py") - 1 );

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptModuleFinder::find_module_code_( PyObject * _module, const ScriptModuleLoaderPtr &  _loader )
	{
		bool successful = this->find_module_( _module, _loader, ".pyz", sizeof(".pyz") - 1, "__init__.pyz", sizeof("__init__.pyz") - 1 );

		return successful;
	}
    //////////////////////////////////////////////////////////////////////////
    bool ScriptModuleFinder::find_module_( PyObject * _module, const ScriptModuleLoaderPtr & _loader, const char * _ext, size_t _extN, const char * _init, size_t _extI )
    {
		char modulePathCache[MENGINE_MAX_PATH];
        
		size_t modulePathCacheLen;
		if( this->convertDotToSlash_( modulePathCache, MENGINE_MAX_PATH, _module, modulePathCacheLen ) == false )
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
    bool ScriptModuleFinder::findModule_( const char * _modulePath, size_t _modulePathLen, const ScriptModuleLoaderPtr & _loader ) const
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
