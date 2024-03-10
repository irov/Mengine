#include "FileService.h"

#include "Interface/ServiceInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/PlatformServiceInterface.h"

#include "Kernel/FileStreamHelper.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/FactoryDefault.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionContainer.h"
#include "Kernel/FilePath.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/PathString.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/AssertionVocabulary.h"

#include "Config/StdString.h"

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
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FileService::_finalizeService()
    {
        MENGINE_ASSERTION_VOCABULARY_EMPTY( STRINGIZE_STRING_LOCAL( "FileGroup" ) );

#if defined(MENGINE_DEBUG)
        for( const HashtableFileGroups::value_type & value : m_fileGroups )
        {
            LOGGER_ASSERTION( "was forgotten unmount '%s'"
                , value.key.c_str()
            );
        }
#endif

        MENGINE_ASSERTION_CONTAINER_EMPTY( m_fileGroups );

        m_fileGroups.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    FileGroupInterfacePtr FileService::createFileGroup( const ConstString & _type, const DocumentInterfacePtr & _doc )
    {
        FactoryInterfacePtr factory = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), _type );

        MENGINE_ASSERTION_MEMORY_PANIC( factory, "not registry factory '%s'"
            , _type.c_str()
        );

        FileGroupInterfacePtr fileGroup = factory->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( fileGroup, "invalid create file group '%s'"
            , _type.c_str()
        );

        return fileGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileService::mountFileGroup( const ConstString & _name, const FileGroupInterfacePtr & _baseFileGroup, const FileGroupInterfacePtr & _parentFileGroup, const FilePath & _filePath, const ConstString & _type, FileGroupInterfacePtr * const _outFileGroup, bool _create, const DocumentInterfacePtr & _doc )
    {
        LOGGER_INFO( "file", "mount file group '%s' path '%s' type '%s'"
            , _name.c_str()
            , _filePath.c_str()
            , _type.c_str()
        );

        MENGINE_ASSERTION_FATAL( m_fileGroups.exist( _name ) == false, "already mount '%s'\n"
            "Remount would be performed"
            , _name.c_str()
        );

        FileGroupInterfacePtr fileGroup = this->createFileGroup( _type, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( fileGroup, "can't create fileGroup '%s' type '%s' for object '%s'"
            , _name.c_str()
            , _type.c_str()
            , _filePath.c_str()
        );

        fileGroup->setName( _name );
        fileGroup->setBaseFileGroup( _baseFileGroup );
        fileGroup->setParentFileGroup( _parentFileGroup );
        fileGroup->setFolderPath( _filePath );

        if( fileGroup->initialize() == false )
        {
            LOGGER_ERROR( "can't initialize fileGroup '%s' for object '%s'"
                , _name.c_str()
                , _filePath.c_str()
            );

            return false;
        }

        if( _create == true )
        {
            const FilePath & relationPath = fileGroup->getRelationPath();
            const FilePath & folderPath = fileGroup->getFolderPath();

            PathString basePath;
            basePath.append( relationPath );
            basePath.append( folderPath );

            if( PLATFORM_SERVICE()
                ->createDirectory( "", basePath.c_str() ) == false )
            {
                LOGGER_ERROR( "invalid create fileGroup '%s' directory '%s'"
                    , _name.c_str()
                    , basePath.c_str()
                );

                return false;
            }
        }

        if( _outFileGroup != nullptr )
        {
            *_outFileGroup = fileGroup;
        }

        m_fileGroups.emplace( _name, fileGroup );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FileService::unmountFileGroup( const ConstString & _name )
    {
        const FileGroupInterfacePtr & groupInterface = m_fileGroups.erase( _name );

        if( groupInterface == nullptr )
        {
            LOGGER_ERROR( "file group '%s' not mount"
                , _name.c_str()
            );
            
            return;
        }

        groupInterface->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileService::hasFileGroup( const ConstString & _name, FileGroupInterfacePtr * const _fileGroup ) const
    {
        const FileGroupInterfacePtr & fileGroup = m_fileGroups.find( _name );

        if( fileGroup == nullptr )
        {
            return false;
        }

        if( _fileGroup != nullptr )
        {
            *_fileGroup = fileGroup;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const FileGroupInterfacePtr & FileService::getFileGroup( const ConstString & _name ) const
    {
        const FileGroupInterfacePtr & fileGroup = m_fileGroups.find( _name );

        MENGINE_ASSERTION_MEMORY_PANIC( fileGroup, "not mount '%s'"
            , _name.c_str()
        );

        return fileGroup;
    }
    //////////////////////////////////////////////////////////////////////////
}
