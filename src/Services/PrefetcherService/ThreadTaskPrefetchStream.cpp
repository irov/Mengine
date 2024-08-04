#include "ThreadTaskPrefetchStream.h"

#include "Kernel/StreamHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/ContentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ThreadTaskPrefetchStream::ThreadTaskPrefetchStream()
        : m_magicNumber( 0 )
        , m_magicVersion( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadTaskPrefetchStream::~ThreadTaskPrefetchStream()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTaskPrefetchStream::setArchivator( const ArchivatorInterfacePtr & _archivator )
    {
        m_archivator = _archivator;
    }
    //////////////////////////////////////////////////////////////////////////
    const ArchivatorInterfacePtr & ThreadTaskPrefetchStream::getArchivator() const
    {
        return m_archivator;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTaskPrefetchStream::setMagicNumber( uint32_t _magicNumber )
    {
        m_magicNumber = _magicNumber;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ThreadTaskPrefetchStream::getMagicNumber() const
    {
        return m_magicNumber;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTaskPrefetchStream::setMagicVersion( uint32_t _magicVersion )
    {
        m_magicVersion = _magicVersion;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ThreadTaskPrefetchStream::getMagicVersion() const
    {
        return m_magicVersion;
    }
    //////////////////////////////////////////////////////////////////////////
    const MemoryInterfacePtr & ThreadTaskPrefetchStream::getMemory() const
    {
        return m_memory;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTaskPrefetchStream::_finalize()
    {
        m_archivator = nullptr;
        m_memory = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadTaskPrefetchStream::_onThreadTaskRun()
    {
        if( ThreadTaskPrefetch::_onThreadTaskRun() == false )
        {
            return false;
        }

        const FileGroupInterfacePtr & fileGroup = m_content->getFileGroup();
        const FilePath & filePath = m_content->getFilePath();

        InputStreamInterfacePtr stream = fileGroup->createInputFile( filePath, false, &m_realFileGroup, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "can't create input stream '%s'"
            , Helper::getContentFullPath( m_content )
        );

        m_stream = stream;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadTaskPrefetchStream::_onThreadTaskProcess()
    {
        LOGGER_INFO( "prefetch", "prefetch stream file '%s'"
            , Helper::getContentFullPath( m_content )
        );

        const FilePath & filePath = m_content->getFilePath();

        if( m_realFileGroup->openInputFile( filePath, m_stream, 0, MENGINE_UNKNOWN_SIZE, false, false ) == false )
        {
            LOGGER_ERROR( "invalid open file '%s'"
                , Helper::getContentFullPath( m_content )
            );

            return false;
        }

        MemoryInterfacePtr memory = Helper::readStreamArchiveMagicMemory( m_stream, m_archivator, m_magicNumber, m_magicVersion, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid stream archive magic memory '%s'"
            , Helper::getContentFullPath( m_content )
        );

        m_memory = memory;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTaskPrefetchStream::_onThreadTaskComplete( bool _successful )
    {
        ThreadTaskPrefetch::_onThreadTaskComplete( _successful );

        m_archivator = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}
