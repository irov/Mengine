#include "FileService.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"

#include "Interface/ServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/VocabularyServiceInterface.h"

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
        for( MapFileGroups::reverse_iterator
            it = m_fileGroups.rbegin(),
            it_end = m_fileGroups.rend();
            it != it_end;
            ++it )
        {
            const FileGroupInterfacePtr & group = it->second;

            group->finalize();
        }

        m_fileGroups.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    FileGroupInterfacePtr FileService::createFileGroup( const ConstString & _type, const Char * _doc )
    {
        FactoryPtr factory = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), _type );

        if( factory == nullptr )
        {
            LOGGER_ERROR( "not registry factory '%s'"
                , _type.c_str()
            );

            return nullptr;
        }

		FileGroupInterfacePtr fileGroup = factory->createObject( _doc );

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
    bool FileService::mountFileGroup( const ConstString & _name, const FileGroupInterfacePtr & _category, const FilePath & _path, const ConstString & _type, FileGroupInterfacePtr * _fileGroup, const Char * _doc )
    {
        LOGGER_INFO( "group '%s' path '%s' type '%s'"
            , _name.c_str()
            , _path.c_str()
            , _type.c_str()
        );

        MapFileGroups::iterator it_find = m_fileGroups.find( _name );

        if( it_find != m_fileGroups.end() )
        {
            LOGGER_ERROR( "already mount '%s'\n"
                "Remount would be performed"
                , _name.c_str()
            );

            return false;
        }

        FileGroupInterfacePtr fileGroup = this->createFileGroup( _type, _doc );

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

        m_fileGroups.emplace( _name, fileGroup );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileService::unmountFileGroup( const ConstString & _name )
    {
        MapFileGroups::iterator it_find = m_fileGroups.find( _name );
        if( it_find == m_fileGroups.end() )
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

        m_fileGroups.erase( it_find );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileService::hasFileGroup( const ConstString& _name, FileGroupInterfacePtr * _fileGroup ) const
    {
        MapFileGroups::const_iterator it_find = m_fileGroups.find( _name );

        if( it_find == m_fileGroups.end() )
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
        MapFileGroups::const_iterator it_find = m_fileGroups.find( _name );

        if( it_find == m_fileGroups.end() )
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
    InputStreamInterfacePtr FileService::openInputFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _fileName, bool _streaming, const Char * _doc )
    {
		InputStreamInterfacePtr file = _fileGroup->createInputFile( _fileName, _streaming, _doc );

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
    OutputStreamInterfacePtr FileService::openOutputFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _fileName, const Char * _doc )
    {
		OutputStreamInterfacePtr file = _fileGroup->createOutputFile( _doc );

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
