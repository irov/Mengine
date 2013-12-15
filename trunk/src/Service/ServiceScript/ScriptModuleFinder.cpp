#   include "ScriptModuleFinder.h"

//#   include "ScriptModuleLoaderSource.h"
//#   include "ScriptModuleLoaderCode.h"

#   include "Interface/StringizeInterface.h"
#	include "Interface/ArchiveInterface.h"
#	include "Interface/CacheInterface.h"


#   include "Config/Blobject.h"

#   include "Logger/Logger.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    static int get_int( unsigned char * _buff )
    {
        int x;
        x =  (int)_buff[0];
        x |= (int)_buff[1] << 8;
        x |= (int)_buff[2] << 16;
        x |= (int)_buff[3] << 24;

        return x;
    }
    //////////////////////////////////////////////////////////////////////////
    ScriptModuleFinder::ScriptModuleFinder( ServiceProviderInterface * _serviceProvider )
        : m_serviceProvider(_serviceProvider)
        , m_embbed(nullptr)
    {
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
    void ScriptModuleFinder::setEmbbed( PyObject * _embbed )
    {
        m_embbed = _embbed;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ScriptModuleFinder::find_module( const ConstString & _module, PyObject * _path )
    {
        (void)_module;
        (void)_path;

#   ifndef MENGE_MASTER_RELEASE
        if( this->find_module_source_( _module ) == true )
        {
            pybind::incref( m_embbed );

            return m_embbed;
        }
#   endif

        if( this->find_module_code_( _module ) == false )
        {
            return pybind::ret_none();
        }

        pybind::incref( m_embbed );

        return m_embbed;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ScriptModuleFinder::load_module( const ConstString & _module )
    {   
		ModulePathCache * pathCache = nullptr;
        if( m_paths.has( _module, &pathCache ) == false )
        {
            return nullptr;
        }        

        //InputStreamInterfacePtr stream = pathCache->fileGroup->createInputFile();

		//m_fullModulePathCache.assign( pathCache->path.c_str(), pathCache->path.size() );
		//m_fullModulePathCache += MENGE_FOLDER_RESOURCE_DELIM;
		//m_fullModulePathCache += pathCache->module;

		//ConstString cs_modulePath = Helper::stringizeString(m_serviceProvider, m_fullModulePathCache);

        //if( pathCache->fileGroup->openInputFile( pathCache->path, stream ) == false )
        //{
        //    return nullptr;
        //}

		InputStreamInterfacePtr stream = pathCache->stream;
		pathCache->stream = nullptr;

		PyObject * module = nullptr;

        if( pathCache->source == true )
        {
            module = this->load_module_source_( _module, stream, pathCache->packagePath );
        }
		else
		{
			module = this->load_module_code_( _module, stream, pathCache->packagePath );
		}

        return module;        
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ScriptModuleFinder::convertDotToSlash_( char * _cache, const ConstString & _module )
    {
		const char * module_str = _module.c_str();
		size_t module_size = _module.size();
		memcpy( _cache, module_str, module_size );

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

		return module_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ScriptModuleFinder::find_module_source_( const ConstString & _module )
    {
		char modulePathCache[MAX_PATH];
        size_t modulePathCacheLen = this->convertDotToSlash_( modulePathCache, _module );

		memcpy( modulePathCache + modulePathCacheLen, ".py", 4 );
        
		//ConstString modulePath = Helper::stringizeString( m_serviceProvider, m_modulePathCache );

        ModulePathCache * pathCache_module = this->findModule_( _module, modulePathCache, modulePathCacheLen + 3 );

        if( pathCache_module != nullptr )
        {
            pathCache_module->packagePath = nullptr;
            pathCache_module->source = true;

            return true;
        }
		else
		{
			modulePathCache[modulePathCacheLen] = MENGE_FOLDER_RESOURCE_DELIM;
			memcpy( modulePathCache + modulePathCacheLen + 1, "__init__.py", 12 );

			ModulePathCache * pathCache_package = this->findModule_( _module, modulePathCache, modulePathCacheLen + 12 );

			if( pathCache_package == nullptr )
			{
				return false;
			}

			//PyObject * py_fullpath = pybind::string_from_char_size( m_modulePathCache.c_str(), m_modulePathCache.size() );
			PyObject * py_fullpath = pybind::ptr( _module );

			pathCache_package->packagePath = pybind::build_value( "[O]", py_fullpath );
			pybind::decref( py_fullpath );

			pathCache_package->source = true;
		}

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ScriptModuleFinder::find_module_code_( const ConstString & _module )
    {
		char modulePathCache[MAX_PATH];
        size_t modulePathCacheLen = this->convertDotToSlash_( modulePathCache, _module );

		memcpy( modulePathCache + modulePathCacheLen, ".pyz", 5 );

        ModulePathCache * pathCache_module = this->findModule_( _module, modulePathCache, modulePathCacheLen + 4 );

        if( pathCache_module != nullptr )
        {
            pathCache_module->packagePath = nullptr;
            pathCache_module->source = false;

            return true;
        }
		else
		{
			modulePathCache[modulePathCacheLen] = MENGE_FOLDER_RESOURCE_DELIM;
			memcpy( modulePathCache + modulePathCacheLen + 1, "__init__.pyz", 13 );

			ModulePathCache * pathCache_package = this->findModule_( _module, modulePathCache, modulePathCacheLen + 13 );

			if( pathCache_package == nullptr )
			{
				return false;
			}

			//PyObject * py_fullpath = pybind::string_from_char_size( m_modulePathCache.c_str(), m_modulePathCache.size() );
			PyObject * py_fullpath = pybind::ptr( _module );

			pathCache_package->packagePath = pybind::build_value( "[O]", py_fullpath );
			pybind::decref( py_fullpath );

			pathCache_package->source = false;
		}

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    ScriptModuleFinder::ModulePathCache * ScriptModuleFinder::findModule_( const ConstString & _module, const char * _modulePath, size_t _modulePathLen ) const
    {
        FileServiceInterface * fileService = FILE_SERVICE(m_serviceProvider);
                
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
                                
                FileGroupInterface * fileGroup;
                if( fileService->existFile( mp.pak, path, _modulePath, _modulePathLen, &fileGroup ) == false )
                {
                    continue;
                }

				InputStreamInterfacePtr stream = fileGroup->createInputFile();

				if( fileGroup->openInputFile( path, _modulePath, _modulePathLen, stream ) == false )
				{
					return nullptr;
				}

                ModulePathCache mpc;
                mpc.stream = stream;

				//m_fullModulePathCache.assign( path.c_str(), path.size() );
				//m_fullModulePathCache += _modulePath;

				//ConstString cs_modulePath = Helper::stringizeString(m_serviceProvider, m_fullModulePathCache);

                //mpc.path = cs_modulePath;

				//InputStreamInterfacePtr stream = pathCache->fileGroup->createInputFile();

				//m_fullModulePathCache.assign( pathCache->path.c_str(), pathCache->path.size() );
				//m_fullModulePathCache += MENGE_FOLDER_RESOURCE_DELIM;
				//m_fullModulePathCache += pathCache->module;

				//ConstString cs_modulePath = Helper::stringizeString(m_serviceProvider, m_fullModulePathCache);
				
                TMapModulePath::iterator it_insert = m_paths.insert( _module, mpc ).first;

				ModulePathCache & pathCache = m_paths.get_value( it_insert );

                return &pathCache;
            }
        }

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ScriptModuleFinder::load_module_source_( const ConstString & _module, const InputStreamInterfacePtr & _stream, PyObject * _packagePath )
    {
        PyObject * code = this->unmarshal_source_( _module, _stream );

        if( code == nullptr )
        {
            return nullptr;
        }

        PyObject * module = pybind::module_init( _module.c_str() );

        PyObject * dict = pybind::module_dict( module );

        pybind::dict_setstring( dict, "__loader__", m_embbed );

        if( _packagePath != nullptr )
        {
            pybind::dict_setstring( dict, "__path__", _packagePath );
            pybind::decref( _packagePath );
        }

        module = pybind::module_execcode( _module.c_str(), code );

        pybind::decref( code );

        return module;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ScriptModuleFinder::load_module_code_( const ConstString & _module, const InputStreamInterfacePtr & _stream, PyObject * _packagePath )
    {
        PyObject * code = this->unmarshal_code_( _module, _stream );

        if( code == nullptr )
        {
            return nullptr;
        }

        PyObject * module = pybind::module_init( _module.c_str() );

        PyObject * dict = pybind::module_dict( module );

        pybind::dict_setstring( dict, "__loader__", m_embbed );

        if( _packagePath != nullptr )
        {
            pybind::dict_setstring( dict, "__path__", _packagePath );
            pybind::decref( _packagePath );
        }

        module = pybind::module_execcode( _module.c_str(), code );

        pybind::decref( code );

        return module;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ScriptModuleFinder::unmarshal_source_( const ConstString & _module, const InputStreamInterfacePtr & _stream )
    {
        size_t file_size = _stream->size();

		CacheBufferInterfacePtr source_buffer = CACHE_SERVICE(m_serviceProvider)
			->lockBuffer( file_size + 2 );

		char * source_memory = source_buffer->getMemoryT<char>();

        if( file_size > 0 )
        {
            _stream->read( source_memory, file_size );
        }

        source_memory[file_size] = '\n';
        source_memory[file_size + 1] = '\0';

        PyObject * code = pybind::code_compile_file( source_memory, _module.c_str() );

        if( code == nullptr )
        {
            pybind::check_error();

            return nullptr;
        }

        if( pybind::code_check( code ) == false ) 
        {
            return nullptr;
        }

        return code;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ScriptModuleFinder::unmarshal_code_( const ConstString & _module, const InputStreamInterfacePtr & _stream )
    {
        size_t file_size = _stream->size();

        if( file_size == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("ScriptModuleFinder::unmarshal_code_ %s zero size"
                , _module.c_str()
                );

            return nullptr;
        }

        size_t code_size;
        _stream->read( &code_size, sizeof(code_size) );

        size_t compress_size;
        _stream->read( &compress_size, sizeof(compress_size) );

		CacheBufferInterfacePtr compress_buffer = CACHE_SERVICE(m_serviceProvider)
			->lockBuffer( compress_size );

		TBlobject::value_type * compress_memory = compress_buffer->getMemory();

		_stream->read( compress_memory, compress_size );
		
		CacheBufferInterfacePtr code_buffer = CACHE_SERVICE(m_serviceProvider)
			->lockBuffer( code_size );

		TBlobject::value_type * code_memory = code_buffer->getMemory();

        size_t uncompress_size;
        if( ARCHIVE_SERVICE(m_serviceProvider)
            ->uncompress( code_memory, code_size, uncompress_size, compress_memory, compress_size ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("ScriptModuleFinder::unmarshal_code_ uncompress failed"
                );

            return nullptr;
        }
                
        long file_magic = get_int( &code_memory[0] );
        long py_magic = pybind::marshal_magic_number();

        if( file_magic != py_magic )
        {
            return nullptr;
        }

        PyObject * code = pybind::marshal_get_object( (char *)code_memory + 8, uncompress_size - 8 );

        if( code == nullptr )
        {
            return nullptr;
        }

        if( pybind::code_check( code ) == false ) 
        {
            return nullptr;
        }

        return code;
    }
}