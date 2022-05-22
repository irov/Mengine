#include "FileModifyHookService.h"

#include "Interface/PlatformInterface.h"
#include "Interface/ThreadServiceInterface.h"

#include "Kernel/ThreadHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionContainer.h"
#include "Kernel/ThreadMutexScope.h"

#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( FileModifyHookService, Mengine::FileModifyHookService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FileModifyHookService::FileModifyHookService()
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
            ThreadServiceInterface::getStaticServiceID()
        };

        return required;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileModifyHookService::_initializeService()
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
    }
    //////////////////////////////////////////////////////////////////////////
    void FileModifyHookService::_finalizeService()
    {
        Helper::destroySimpleThreadWorker( STRINGIZE_STRING_LOCAL( "FileModifyHook" ) );

        m_fileModifyMutex = nullptr;

        MENGINE_ASSERTION_CONTAINER_EMPTY( m_fileModifies );

        m_fileModifies.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileModifyHookService::setFileModifyHook( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const LambdaFileModifyHook & _lambda )
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
    void FileModifyHookService::removeFileModifyHook( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath )
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
    void FileModifyHookService::notifyFileModifies() const
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
    void FileModifyHookService::checkFileModifies() const
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