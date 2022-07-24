#pragma once

#include "Interface/ArchivatorInterface.h"

#include "ThreadTaskPrefetch.h"

namespace Mengine
{
    class ThreadTaskPrefetchStream
        : public ThreadTaskPrefetch
    {
        DECLARE_FACTORABLE( ThreadTaskPrefetchStream );

    public:
        ThreadTaskPrefetchStream();
        ~ThreadTaskPrefetchStream() override;

    public:
        void setArchivator( const ArchivatorInterfacePtr & _archivator );
        const ArchivatorInterfacePtr & getArchivator() const;

    public:
        void setMagicNumber( uint32_t _magicNumber );
        uint32_t getMagicNumber() const;

        void setMagicVersion( uint32_t _magicVersion );
        uint32_t getMagicVersion() const;

    public:
        const MemoryInterfacePtr & getMemory() const;

    protected:
        void _finalize() override;

    protected:
        bool _onThreadTaskRun() override;
        bool _onThreadTaskProcess() override;
        void _onThreadTaskComplete( bool _successful ) override;

    protected:
        ArchivatorInterfacePtr m_archivator;

        uint32_t m_magicNumber;
        uint32_t m_magicVersion;

    protected:
        MemoryInterfacePtr m_memory;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadTaskPrefetchStream, ThreadTaskInterface> ThreadTaskPrefetchStreamPtr;
    //////////////////////////////////////////////////////////////////////////
}