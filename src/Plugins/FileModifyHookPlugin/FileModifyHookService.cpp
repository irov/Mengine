#include "FileModifyHookService.h"

#include "Interface/ThreadServiceInterface.h"
#include "Interface/TimerServiceInterface.h"
#include "Interface/FileSystemInterface.h"

#include "Kernel/ThreadHelper.h"
#include "Kernel/AssertionContainer.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/ThreadMutexHelper.h"
#include "Kernel/ConfigHelper.h"

#include "Config/StdString.h"
#include "Config/Path.h"

#define FILEMODIFYHOOK_THREAD_NAME "FileModifyHook"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( FileModifyHookService, Mengine::FileModifyHookService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FileModifyHookService::FileModifyHookService()
        : m_timerId( INVALID_UNIQUE_ID )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FileModifyHookService::~FileModifyHookService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const ServiceRequiredList & FileModifyHookService::requiredServices() const
    {
        static ServiceRequiredList required = {
            SERVICE_ID( ThreadServiceInterface )
        };

        return required;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileModifyHookService::_initializeService()
    {
        ThreadMutexInterfacePtr mutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        m_mutex = mutex;

        uint32_t FileModifyHook_Time = CONFIG_VALUE_INTEGER( "FileModifyHookPlugin", "Time", 500 );

        ThreadIdentityInterfacePtr thread = Helper::createThreadIdentity( MENGINE_THREAD_DESCRIPTION( "MNGFileModify" ), ETP_BELOW_NORMAL, [this]( const ThreadIdentityRunnerInterfacePtr & _runner )
        {
            this->process( _runner );

            return true;
        }, FileModifyHook_Time, MENGINE_DOCUMENT_FACTORABLE );

        m_thread = thread;

        UniqueId timerId = TIMER_SERVICE()
            ->addTimer( FileModifyHook_Time, [this]( UniqueId _id )
        {
            MENGINE_UNUSED( _id );

            this->notifyFileModifies();
        }, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_FATAL( timerId != INVALID_UNIQUE_ID, "invalid add timer" );

        m_timerId = timerId;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FileModifyHookService::_finalizeService()
    {
        if( m_thread != nullptr )
        {
            m_thread->join( true );
            m_thread = nullptr;
        }

        if( m_timerId != INVALID_UNIQUE_ID )
        {
            TIMER_SERVICE()
                ->removeTimer( m_timerId );
            m_timerId = INVALID_UNIQUE_ID;
        }

        m_mutex = nullptr;

        MENGINE_ASSERTION_CONTAINER_EMPTY( m_fileModifies );

        m_fileModifies.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileModifyHookService::setFileModifyHook( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const LambdaFileModifyHook & _lambda )
    {
        FileModifyDesc desc;
        _fileGroup->getFullPath( _filePath, desc.fullPath );

        desc.lambda = _lambda;

        desc.time = FILE_SYSTEM()
            ->getFileTime( desc.fullPath );

        desc.modify = false;

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        m_fileModifies.emplace_back( desc );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FileModifyHookService::removeFileModifyHook( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath )
    {
        Path fullPath = {'\0'};
        _fileGroup->getFullPath( _filePath, fullPath );

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        for( VectorFileModifyDesc::iterator
            it = m_fileModifies.begin(),
            it_end = m_fileModifies.end();
            it != it_end;
            ++it )
        {
            const FileModifyDesc & desc = *it;

            if( StdString::strcmp( desc.fullPath, fullPath ) != 0 )
            {
                continue;
            }

            *it = m_fileModifies.back();
            m_fileModifies.pop_back();

            break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void FileModifyHookService::notifyFileModifies() const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

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
    void FileModifyHookService::process( const ThreadIdentityRunnerInterfacePtr & _runner ) const
    {
        MENGINE_UNUSED( _runner );

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        for( const FileModifyDesc & desc : m_fileModifies )
        {
            uint64_t time = FILE_SYSTEM()
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