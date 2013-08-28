#   include "ScriptModuleFinder.h"

//#   include "ScriptModuleLoaderSource.h"
//#   include "ScriptModuleLoaderCode.h"

#   include "Interface/StringizeInterface.h"
#	include "Interface/ArchiveInterface.h"


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

        InputStreamInterfacePtr stream = pathCache->fileGroup->createInputFile();

        if( pathCache->fileGroup->openInputFile( pathCache->path, stream ) == false )
        {
            return nullptr;
        }

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
    void ScriptModuleFinder::convertDotToSlash_( const ConstString & _module )
    {
        m_modulePathCache.assign( _module.c_str(), _module.size() );

        for( String::iterator 
            it = m_modulePathCache.begin(),
            it_end = m_modulePathCache.end();
        it != it_end;
        ++it )
        {
            if( *it == '.' )
            {
                *it = MENGE_FOLDER_RESOURCE_DELIM;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool ScriptModuleFinder::find_module_source_( const ConstString & _module )
    {
        this->convertDotToSlash_( _module );

        m_modulePathCache += ".py";
        
        ModulePathCache * pathCache_module = this->findModule_( _module, m_modulePathCache );

        if( pathCache_module != nullptr )
        {
            pathCache_module->packagePath = nullptr;
            pathCache_module->source = true;

            return true;
        }

        m_modulePathCache.erase( m_modulePathCache.size() - 3, 3 );

        m_modulePathCache += MENGE_FOLDER_RESOURCE_DELIM;

        m_modulePathCache += "__init__";
        m_modulePathCache += ".py";
        
        ModulePathCache * pathCache_package = this->findModule_( _module, m_modulePathCache );

        if( pathCache_package == nullptr )
        {
            return false;
        }

        PyObject * py_fullpath = pybind::string_from_char_size( m_modulePathCache.c_str(), m_modulePathCache.size() );

        pathCache_package->packagePath = pybind::build_value( "[O]", py_fullpath );
        pybind::decref( py_fullpath );

        pathCache_package->source = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ScriptModuleFinder::find_module_code_( const ConstString & _module )
    {
        this->convertDotToSlash_( _module );

        m_modulePathCache += ".pyz";

        ModulePathCache * pathCache_module = this->findModule_( _module, m_modulePathCache );

        if( pathCache_module != nullptr )
        {
            pathCache_module->packagePath = nullptr;
            pathCache_module->source = false;

            return true;
        }
         
        m_modulePathCache.erase( m_modulePathCache.size() - 4, 4 );

        m_modulePathCache += MENGE_FOLDER_RESOURCE_DELIM;

        m_modulePathCache += "__init__";
        m_modulePathCache += ".pyz";

        ModulePathCache * pathCache_package = this->findModule_( _module, m_modulePathCache );

        if( pathCache_package == nullptr )
        {
            return false;
        }

        PyObject * py_fullpath = pybind::string_from_char_size( m_modulePathCache.c_str(), m_modulePathCache.size() );

        pathCache_package->packagePath = pybind::build_value( "[O]", py_fullpath );
        pybind::decref( py_fullpath );

        pathCache_package->source = false;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    ScriptModuleFinder::ModulePathCache * ScriptModuleFinder::findModule_( const ConstString & _module, const String & _modulePath ) const
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
                                
                m_fullModulePathCache.assign( path.c_str(), path.size() );
                m_fullModulePathCache += MENGE_FOLDER_RESOURCE_DELIM;
                m_fullModulePathCache += _modulePath;

                ConstString cs_modulePath = Helper::stringizeString(m_serviceProvider, m_fullModulePathCache);
                
                FileGroupInterface * fileGroup;
                if( fileService->existFile( mp.pak, cs_modulePath, &fileGroup ) == false )
                {
                    continue;
                }

                InputStreamInterfacePtr stream = fileGroup->createInputFile();

                if( stream == nullptr )
                {
                    return nullptr;
                }

                ModulePathCache mpc;
                mpc.fileGroup = fileGroup;
                mpc.path = cs_modulePath;

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

        typedef std::vector<char> TVectorChar; 
        static TVectorChar buf;

        buf.resize( file_size + 2 );

        if( file_size > 0 )
        {
            _stream->read( &buf[0], file_size );
        }

        buf[file_size] = '\n';
        buf[file_size + 1] = '\0';

        PyObject * code = pybind::code_compile_file( &buf[0], _module.c_str() );

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

        static TBlobject compress_buf;

        compress_buf.resize( compress_size );

        _stream->read( &compress_buf[0], compress_size );

        static TBlobject code_buf;

        code_buf.resize( code_size );

        size_t uncompress_size;
        if( ARCHIVE_SERVICE(m_serviceProvider)
            ->uncompress( &code_buf[0], code_size, uncompress_size, &compress_buf[0], compress_size ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("ScriptModuleFinder::unmarshal_code_ uncompress failed"
                );

            return nullptr;
        }
                
        long file_magic = get_int( &code_buf[0] );
        long py_magic = pybind::marshal_magic_number();

        if( file_magic != py_magic )
        {
            return nullptr;
        }

        PyObject * code = pybind::marshal_get_object( (char *)&code_buf[0] + 8, uncompress_size - 8 );

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