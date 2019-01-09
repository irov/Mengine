#include "FileService.h"

#include "Kernel/Logger.h"

#include "Interface/ServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/FactoryDefault.h"

#include "Kernel/String.h"
#include "Kernel/FilePath.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( FileService, Mengine::FileService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FileService::FileService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FileService::~FileService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileService::_initializeService()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FileService::_finalizeService()
    {
        for( MapFileSystem::reverse_iterator
            it = m_fileSystemMap.rbegin(),
            it_end = m_fileSystemMap.rend();
            it != it_end;
            ++it )
        {
            const FileGroupInterfacePtr & group = it->second;

            group->finalize();
        }

        m_factoryFileGroups.clear();
        m_fileSystemMap.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void FileService::registerFileGroupFactory( const ConstString & _type, const FactoryPtr & _factory )
    {
        m_factoryFileGroups.emplace( _type, _factory );
    }
    //////////////////////////////////////////////////////////////////////////
    void FileService::unregisterFileGroupFactory( const ConstString & _type )
    {
        TFactoryFileGroups::iterator it_found = m_factoryFileGroups.find( _type );

        if( it_found == m_factoryFileGroups.end() )
        {
            LOGGER_ERROR( "not registry factory '%s'"
                , _type.c_str()
            );

            return;
        }

        m_factoryFileGroups.erase( it_found );
    }
    //////////////////////////////////////////////////////////////////////////
    FileGroupInterfacePtr FileService::createFileGroup( const ConstString & _type )
    {
        TFactoryFileGroups::iterator it_found = m_factoryFileGroups.find( _type );

        if( it_found == m_factoryFileGroups.end() )
        {
            LOGGER_ERROR( "not registry factory '%s'"
                , _type.c_str()
            );

            return nullptr;
        }

        const FactoryPtr & factory = it_found->second;

        FileGroupInterfacePtr fileGroup = factory->createObject();

        if( fileGroup == nullptr )
        {
            LOGGER_ERROR( "invalid create file group '%s'"
                , _type.c_str()
            );

            return nullptr;
        }

        return fileGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileService::mountFileGroup( const ConstString & _name, const FileGroupInterfacePtr & _category, const FilePath & _path, const ConstString & _type, FileGroupInterfacePtr * _fileGroup )
    {
        LOGGER_INFO( "group '%s' path '%s' type '%s'"
            , _name.c_str()
            , _path.c_str()
            , _type.c_str()
        );

        MapFileSystem::iterator it_find = m_fileSystemMap.find( _name );

        if( it_find != m_fileSystemMap.end() )
        {
            LOGGER_ERROR( "already mount '%s'\n"
                "Remount would be performed"
                , _name.c_str()
            );

            return false;
        }

        FileGroupInterfacePtr fileGroup = this->createFileGroup( _type );

        if( fileGroup == nullptr )
        {
            LOGGER_ERROR( "can't create fileGroup '%s' type '%s' for object '%s'"
                , _name.c_str()
                , _type.c_str()
                , _path.c_str()
            );

            return false;
        }

        if( fileGroup->initialize( _name, _category, _path ) == false )
        {
            LOGGER_ERROR( "can't initialize FileSystem '%s' for object '%s'"
                , _name.c_str()
                , _path.c_str()
            );

            return false;
        }

        if( _name.empty() == true )
        {
            m_defaultFileGroup = fileGroup;
        }

        if( _fileGroup != nullptr )
        {
            *_fileGroup = fileGroup;
        }

        m_fileSystemMap.emplace( _name, fileGroup );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileService::unmountFileGroup( const ConstString & _name )
    {
        MapFileSystem::iterator it_find = m_fileSystemMap.find( _name );
        if( it_find == m_fileSystemMap.end() )
        {
            LOGGER_ERROR( "not mount '%s'"
                , _name.c_str()
            );

            return false;
        }

        const FileGroupInterfacePtr & groupInterface = it_find->second;

        groupInterface->finalize();

        if( _name.empty() == true )
        {
            m_defaultFileGroup = nullptr;
        }

        m_fileSystemMap.erase( it_find );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileService::hasFileGroup( const ConstString& _name, FileGroupInterfacePtr * _fileGroup ) const
    {
        MapFileSystem::const_iterator it_find = m_fileSystemMap.find( _name );

        if( it_find == m_fileSystemMap.end() )
        {
            return false;
        }

        if( _fileGroup != nullptr )
        {
            *_fileGroup = it_find->second;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const FileGroupInterfacePtr & FileService::getFileGroup( const ConstString & _name ) const
    {
        MapFileSystem::const_iterator it_find = m_fileSystemMap.find( _name );

        if( it_find == m_fileSystemMap.end() )
        {
            LOGGER_ERROR( "not mount '%s'"
                , _name.c_str()
            );

            return FileGroupInterfacePtr::none();
        }

        const FileGroupInterfacePtr & fileGroup = it_find->second;

        return fileGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    const FileGroupInterfacePtr & FileService::getDefaultFileGroup() const
    {
        return m_defaultFileGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterfacePtr FileService::openInputFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _fileName, bool _streaming )
    {
        InputStreamInterfacePtr file = _fileGroup->createInputFile( _fileName, _streaming );

        if( file == nullptr )
        {
            LOGGER_ERROR( "can't create input file '%s:%s'"
                , _fileGroup->getName().c_str()
                , _fileName.c_str()
            );

            return nullptr;
        }

        if( _fileGroup->openInputFile( _fileName, file, 0, 0, _streaming ) == false )
        {
            LOGGER_ERROR( "can't open input file '%s:%s'"
                , _fileGroup->getName().c_str()
                , _fileName.c_str()
            );

            return nullptr;
        }

        return file;
    }
    //////////////////////////////////////////////////////////////////////////
    OutputStreamInterfacePtr FileService::openOutputFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _fileName )
    {
        OutputStreamInterfacePtr file = _fileGroup->createOutputFile();

        if( file == nullptr )
        {
            LOGGER_ERROR( "can't create output file '%s:%s'"
                , _fileGroup->getName().c_str()
                , _fileName.c_str()
            );

            return nullptr;
        }

        if( _fileGroup->openOutputFile( _fileName, file ) == false )
        {
            LOGGER_ERROR( "can't open output file '%s:%s'"
                , _fileGroup->getName().c_str()
                , _fileName.c_str()
            );

            return nullptr;
        }

        return file;
    }
}
