#   include "ScriptModuleFinder.h"

#   include "Interface/StringizeInterface.h"
#	include "Interface/ArchiveInterface.h"
#	include "Interface/CacheInterface.h"

#	include "Core/CacheMemoryBuffer.h"

#   include "Config/Blobject.h"

#   include "Logger/Logger.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    ScriptModuleFinder::ScriptModuleFinder()
        : m_serviceProvider(nullptr)
		, m_embed(nullptr)
    {
    }
	//////////////////////////////////////////////////////////////////////////
	bool ScriptModuleFinder::initialize( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;

		m_archivator = ARCHIVE_SERVICE(m_serviceProvider)
			->getArchivator( CONST_STRING_LOCAL(m_serviceProvider, "zip") );

		if( m_archivator == nullptr )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptModuleFinder::finalize()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptModuleFinder::setEmbed( PyObject * _embed )
	{
		m_embed = _embed;
	}
    //////////////////////////////////////////////////////////////////////////
    void ScriptModuleFinder::addModulePath( const ConstString & _pak, const TVectorFilePath& _pathes )
    {
        ModulePathes mp;
        mp.pak = _pak;
        mp.pathes = _pathes;

        m_modulePaths.push_back( mp );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ScriptModuleFinder::find_module( PyObject * _module, PyObject * _path )
    {
        (void)_module;
        (void)_path;

#   ifndef MENGE_MASTER_RELEASE
		{
			ScriptModuleLoaderPtr loaderSource = m_factoryScriptModuleLoaderSource.createObjectT();

			loaderSource->setServiceProvider( m_serviceProvider );
			loaderSource->setModule( _module );

			if( this->find_module_source_( _module, loaderSource ) == true )
			{
				m_loaders.push_back( loaderSource );

				pybind::incref( m_embed );
				return m_embed;
			}
		}
#   endif
	
		{
			ScriptModuleLoaderPtr loaderCode = m_factoryScriptModuleLoaderCode.createObjectT();

			loaderCode->setServiceProvider( m_serviceProvider );
			loaderCode->setModule( _module );

			if( this->find_module_code_( _module, loaderCode ) == true )
			{
				m_loaders.push_back( loaderCode );

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

		if( loader_module != _module )
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

        for( char
            *it = _cache,
            *it_end = _cache + module_size;
        it != it_end;
        ++it )
        {
            if( *it == '.' )
            {
                *it = MENGE_FOLDER_RESOURCE_DELIM;
            }
        }

		_modulePathCacheLen = module_size;
		
		return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool ScriptModuleFinder::find_module_source_( PyObject * _module, const ScriptModuleLoaderPtr & _loader )
	{
		bool successful = this->find_module_( _module, _loader, ".py", sizeof(".py"), "__init__.py", sizeof("__init__.py") );

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptModuleFinder::find_module_code_( PyObject * _module, const ScriptModuleLoaderPtr &  _loader )
	{
		bool successful = this->find_module_( _module, _loader, ".pyz", sizeof(".pyz"), "__init__.pyz", sizeof("__init__.pyz") );

		return successful;
	}
    //////////////////////////////////////////////////////////////////////////
    bool ScriptModuleFinder::find_module_( PyObject * _module, const ScriptModuleLoaderPtr & _loader, const char * _ext, size_t _extN, const char * _init, size_t _extI )
    {
		char modulePathCache[MAX_PATH];
        
		size_t modulePathCacheLen;
		if( this->convertDotToSlash_( modulePathCache, MAX_PATH, _module, modulePathCacheLen ) == false )
		{
			return false;
		}

		if( stdex::memorycopy_safe( modulePathCache, modulePathCacheLen, MAX_PATH, _ext, _extN ) == false )
		{
			return false;
		}
		
        if( this->findModule_( modulePathCache, modulePathCacheLen + _extN, _loader ) == false )
		{
			modulePathCache[modulePathCacheLen] = MENGE_FOLDER_RESOURCE_DELIM;
			if( stdex::memorycopy_safe( modulePathCache, modulePathCacheLen + 1, MAX_PATH, _init, _extI ) == false )
			{
				return false;
			}

			if( this->findModule_( modulePathCache, modulePathCacheLen + _extI, _loader ) == false )
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
        for( TVectorModulePathes::const_iterator
            it = m_modulePaths.begin(),
            it_end = m_modulePaths.end();
        it != it_end;
        ++it )
        {
            const ModulePathes & mp = *it;

            for( TVectorFilePath::const_iterator
                it_path = mp.pathes.begin(),
                it_path_end = mp.pathes.end();
            it_path != it_path_end;
            ++it_path )
            {
                const FilePath & path = *it_path;
				
				PathString & fullPath = _loader->modifyPath();
				fullPath += path;
				fullPath.append( _modulePath, _modulePathLen );

				//ConstString c_fullPath = Helper::stringizeString( m_serviceProvider, fullPath );
				ConstStringHolderLocal holder_fullPath_local(fullPath.c_str(), fullPath.size());
				ConstString c_fullPath_local(&holder_fullPath_local);

				FileGroupInterfacePtr fileGroup;
                if( FILE_SERVICE(m_serviceProvider)->existFile( mp.pak, c_fullPath_local, &fileGroup ) == false )
                {
                    continue;
                }

				_loader->initialize( fileGroup, m_archivator );
		
				return true;
            }
        }

        return false;
    }
}