#include "FileService.h"

#include "Interface/ServiceInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/PlatformInterface.h"

#include "Kernel/FileStreamHelper.h"
#include "Kernel/ThreadHelper.h"
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
        //ToDo move to dev plugin [https://github.com/irov/Mengine/issues/95]

#ifdef MENGINE_WINDOWS_DEBUG
        SERVICE_WAIT( ThreadServiceInterface, [this]()
        {
            Helper::createSimpleThreadWorker( STRINGIZE_STRING_LOCAL( "FileModifyHook" ), ETP_BELOW_NORMAL, 500, [this]()
            {
                this->notifyFileModifies();
            }, [this]()
            {
                this->checkFileModifies();
            }, MENGINE_DOCUMENT_FACTORABLE );

            ThreadMutexInterfacePtr fileModifyMutex = THREAD_SERVICE()
                ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

            m_fileModifyMutex = fileModifyMutex;

            return true;
        } );

        SERVICE_LEAVE( ThreadServiceInterface, [this]()
        {
            Helper::destroySimpleThreadWorker( STRINGIZE_STRING_LOCAL( "FileModifyHook" ) );

            m_fileModifyMutex = nullptr;
        } );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FileService::_finalizeService()
    {
        MENGINE_ASSERTION_CONTAINER_EMPTY( m_fileModifies );

        m_fileModifies.clear();

        m_defaultFileGroup = nullptr;
        m_globalFileGroup = nullptr;

#ifdef MENGINE_DEBUG
        for( const HashtableFileGroups::value_type & value : m_fileGroups )
        {
            LOGGER_ERROR( "was forgotten unmount '%s'"
                , value.key.c_str()
            );
        }
#endif

        MENGINE_ASSERTION_CONTAINER_EMPTY( m_fileGroups );

        m_fileGroups.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    FileGroupInterfacePtr FileService::createFileGroup( const ConstString & _type, const DocumentPtr & _doc )
    {
        FactoryPtr factory = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), _type );

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
    bool FileService::mountFileGroup( const ConstString & _name, const FileGroupInterfacePtr & _baseFileGroup, const FileGroupInterfacePtr & _parentFileGroup, const FilePath & _filePath, const ConstString & _type, FileGroupInterfacePtr * const _outFileGroup, bool _create, const DocumentPtr & _doc )
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

        if( _name.empty() == true )
        {
            m_defaultFileGroup = fileGroup;
        }
        else if( _name == STRINGIZE_STRING_LOCAL( "dev" ) )
        {
            m_globalFileGroup = fileGroup;
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

        MENGINE_ASSERTION_MEMORY_PANIC( groupInterface, "not mount '%s'"
            , _name.c_str()
        );

        groupInterface->finalize();

        if( _name.empty() == true )
        {
            m_defaultFileGroup = nullptr;
        }
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
    void FileService::setDefaultFileGroup( const FileGroupInterfacePtr & _fileGroup )
    {
        m_defaultFileGroup = _fileGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    const FileGroupInterfacePtr & FileService::getDefaultFileGroup() const
    {
        return m_defaultFileGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    void FileService::setGlobalFileGroup( const FileGroupInterfacePtr & _fileGroup )
    {
        m_globalFileGroup = _fileGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    const FileGroupInterfacePtr & FileService::getGlobalFileGroup() const
    {
        return m_globalFileGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileService::setFileModifyHook( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const LambdaFileModifyHook & _lambda )
    {
        FileModifyDesc desc;

        _fileGroup->getFullPath( _filePath, desc.fullPath );

        desc.lambda = _lambda;

        desc.time = PLATFORM_SERVICE()
            ->getFileTime( desc.fullPath );

        desc.modify = false;

        MENGINE_THREAD_MUTEX_SCOPE( m_fileModifyMutex );

        m_fileModifies.emplace_back( desc );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FileService::removeFileModifyHook( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath )
    {
        Char fullPath[MENGINE_MAX_PATH] = {'\0'};
        _fileGroup->getFullPath( _filePath, fullPath );

        MENGINE_THREAD_MUTEX_SCOPE( m_fileModifyMutex );

        for( VectorFileModifyDesc::iterator
            it = m_fileModifies.begin(),
            it_end = m_fileModifies.end();
            it != it_end;
            ++it )
        {
            const FileModifyDesc & desc = *it;

            if( MENGINE_STRCMP( desc.fullPath, fullPath ) != 0 )
            {
                continue;
            }

            *it = m_fileModifies.back();
            m_fileModifies.pop_back();

            break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void FileService::notifyFileModifies() const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_fileModifyMutex );

        for( const FileModifyDesc & desc : m_fileModifies )
        {
            if( desc.modify == false )
            {
                continue;
            }

            desc.modify = false;
            desc.lambda();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void FileService::checkFileModifies() const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_fileModifyMutex );

        for( const FileModifyDesc & desc : m_fileModifies )
        {
            uint64_t time = PLATFORM_SERVICE()
                ->getFileTime( desc.fullPath );

            if( desc.time == time )
            {
                continue;
            }

            desc.time = time;
            desc.modify = true;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
