#include "ThreadTaskPrefetchStream.h"

#include "Interface/CodecInterface.h"

#include "Kernel/Stream.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

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

        m_stream = m_fileGroup->createInputFile( m_filePath, false, &m_realFileGroup, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( m_stream, "can't create input file '%s'"
            , this->getFileGroup()->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadTaskPrefetchStream::_onThreadTaskProcess()
    {
        if( m_realFileGroup->openInputFile( m_filePath, m_stream, 0, ~0U, false, false ) == false )
        {
            LOGGER_ERROR( "invalid open file '%s':'%s'"
                , this->getFileGroup()->getName().c_str()
                , this->getFilePath().c_str()
            );

            return false;
        }

        MemoryInterfacePtr memory = Helper::loadStreamArchiveMagicMemory( m_stream, m_archivator, m_magicNumber, m_magicVersion, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid stream archive magic memory '%s':'%s'"
            , this->getFileGroup()->getName().c_str()
            , this->getFilePath().c_str()
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
