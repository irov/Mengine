#   include "ScriptModuleFinder.h"

#   include "ScriptModuleLoaderSource.h"
#   include "ScriptModuleLoaderCode.h"

#   include "Interface/FileSystemInterface.h"
#   include "Interface/StringizeInterface.h"

#   include "Logger/Logger.h"

#	include <pybind/../config/python.hpp>

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    ScriptModuleFinder::ScriptModuleFinder( ServiceProviderInterface * _serviceProvider )
        : m_serviceProvider(_serviceProvider)
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
    PyObject * ScriptModuleFinder::find_module( const char * _module, PyObject * _path )
    {
        (void)_path;
#   ifndef MENGE_MASTER_RELEASE
        PyObject * loader = find_module_source_( _module );

        if( pybind::is_none( loader ) == true )
        {
            loader = find_module_code_( _module );
        }
#   else
        PyObject * loader = find_module_code_( _module );
#   endif

        return loader;             
    }
    //////////////////////////////////////////////////////////////////////////
    void ScriptModuleFinder::convertDotToSlash_( const char * _module )
    {
        m_modulePathCache = _module;

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
    PyObject * ScriptModuleFinder::find_module_source_( const char * _module )
    {
        this->convertDotToSlash_( _module );

        m_modulePathCache += ".py";
        
        PyObject * packagePath = NULL;

        InputStreamInterface * stream = this->findModule( m_modulePathCache );

        if( stream == NULL )
        {
            m_modulePathCache.erase( m_modulePathCache.size() - 3, 3 );

            m_modulePathCache += MENGE_FOLDER_RESOURCE_DELIM;

            m_modulePathCache += "__init__";
            m_modulePathCache += ".py";

            stream = this->findModule( m_modulePathCache );

            if( stream == NULL )
            {
                return pybind::ret_none();
            }

            PyObject * py_fullpath = PyString_FromStringAndSize( m_modulePathCache.c_str(), m_modulePathCache.size() );

            packagePath = Py_BuildValue( "[O]", py_fullpath );
            pybind::decref( py_fullpath );
        }

        ScriptModuleLoaderSource * loader = new ScriptModuleLoaderSource(m_serviceProvider, stream, packagePath);

        pybind::decref( packagePath );

        PyObject * py_loader = pybind::ptr( loader );

        loader->setEmbbed( py_loader );

        return py_loader;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ScriptModuleFinder::find_module_code_( const char * _module )
    {
        this->convertDotToSlash_( _module );

        m_modulePathCache += ".pyo";

        PyObject * packagePath = NULL;

        InputStreamInterface * stream = this->findModule( m_modulePathCache );

        if( stream == NULL )
        {
            m_modulePathCache.erase( m_modulePathCache.size() - 4, 4 );

            m_modulePathCache += MENGE_FOLDER_RESOURCE_DELIM;

            m_modulePathCache += "__init__";
            m_modulePathCache += ".pyo";

            stream = this->findModule( m_modulePathCache );

            if( stream == NULL )
            {
                return pybind::ret_none();
            }

            PyObject * py_fullpath = PyString_FromStringAndSize( m_modulePathCache.c_str(), m_modulePathCache.size() );

            packagePath = Py_BuildValue( "[O]", py_fullpath );
            pybind::decref( py_fullpath );
        }

        ScriptModuleLoaderCode * loader = new ScriptModuleLoaderCode(m_serviceProvider, stream, packagePath);

        pybind::decref( packagePath );

        PyObject * py_loader = pybind::ptr( loader );

        loader->setEmbbed( py_loader );

        return py_loader;
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterface * ScriptModuleFinder::findModule( const String & _modulePath ) const
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

                InputStreamInterface * stream = fileGroup->createInputFile();

                if( stream == NULL )
                {
                    return NULL;
                }

                if( fileGroup->openInputFile( cs_modulePath, stream ) == false )
                {
                    stream->destroy();

                    return NULL;
                }

                return stream;
            }
        }

        return NULL;
    }
}