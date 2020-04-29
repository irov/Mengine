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
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/FilePath.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"

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
        SERVICE_WAIT( ThreadServiceInterface, [this]()
        {
            Helper::createSimpleThreadWorker( STRINGIZE_STRING_LOCAL( "FileModifyHook" ), 500, [this]()
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

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FileService::_finalizeService()
    {
        m_fileModifies.clear();

        m_defaultFileGroup = nullptr;

        for( HashtableFileGroups::const_reverse_iterator
            it = m_fileGroups.rbegin(),
            it_end = m_fileGroups.rend();
            it != it_end;
            ++it )
        {
            const FileGroupInterfacePtr & group = it->element;

            group->finalize();
        }

        m_fileGroups.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    FileGroupInterfacePtr FileService::createFileGroup( const ConstString & _type, const DocumentPtr & _doc )
    {
        FactoryPtr factory = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), _type );

        MENGINE_ASSERTION_MEMORY_PANIC( factory, nullptr, "not registry factory '%s'"
            , _type.c_str()
        );

        FileGroupInterfacePtr fileGroup = factory->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( fileGroup, nullptr, "invalid create file group '%s'"
            , _type.c_str()
        );

        return fileGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileService::mountFileGroup( const ConstString & _name, const FileGroupInterfacePtr & _baseFileGroup, const FileGroupInterfacePtr & _parentFileGroup, const FilePath & _filePath, const ConstString & _type, FileGroupInterfacePtr * _outFileGroup, bool _create, const DocumentPtr & _doc )
    {
        LOGGER_INFO( "mount file group '%s' path '%s' type '%s'"
            , _name.c_str()
            , _filePath.c_str()
            , _type.c_str()
        );

        MENGINE_ASSERTION_RETURN( m_fileGroups.exist( _name ) == false, false, "already mount '%s'\n"
            "Remount would be performed"
            , _name.c_str()
        );

        FileGroupInterfacePtr fileGroup = this->createFileGroup( _type, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( fileGroup, false, "can't create fileGroup '%s' type '%s' for object '%s'"
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
            if( fileGroup->createDirectory( FilePath::none() ) == false )
            {
                LOGGER_ERROR( "invalid create fileGroup '%s' directory '%s'"
                    , _name.c_str()
                    , _filePath.c_str()
                );

                return false;
            }
        }

        if( _name.empty() == true )
        {
            m_defaultFileGroup = fileGroup;
        }

        if( _outFileGroup != nullptr )
        {
            *_outFileGroup = fileGroup;
        }

        m_fileGroups.emplace( _name, fileGroup );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileService::unmountFileGroup( const ConstString & _name )
    {
        const FileGroupInterfacePtr & groupInterface = m_fileGroups.erase( _name );

        MENGINE_ASSERTION_MEMORY_PANIC( groupInterface, false, "not mount '%s'"
            , _name.c_str()
        );

        groupInterface->finalize();

        if( _name.empty() == true )
        {
            m_defaultFileGroup = nullptr;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileService::hasFileGroup( const ConstString & _name, FileGroupInterfacePtr * _fileGroup ) const
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

        MENGINE_ASSERTION_MEMORY_PANIC( fileGroup, FileGroupInterfacePtr::none(), "not mount '%s'"
            , _name.c_str()
        );

        return fileGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    const FileGroupInterfacePtr & FileService::getDefaultFileGroup() const
    {
        return m_defaultFileGroup;
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
        Char fullPath[MENGINE_MAX_PATH];
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
}
