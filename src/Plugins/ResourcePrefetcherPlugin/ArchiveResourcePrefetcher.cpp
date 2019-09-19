#include "ArchiveResourcePrefetcher.h"

#include "Interface/PrefetcherServiceInterface.h"

#include "Kernel/Content.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ArchiveResourcePrefetcher::ArchiveResourcePrefetcher()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ArchiveResourcePrefetcher::~ArchiveResourcePrefetcher()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ArchiveResourcePrefetcher::setArchivator( const ArchivatorInterfacePtr & _archivator )
    {
        m_archivator = _archivator;
    }
    //////////////////////////////////////////////////////////////////////////
    const ArchivatorInterfacePtr & ArchiveResourcePrefetcher::getArchivator() const
    {
        return m_archivator;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ArchiveResourcePrefetcher::_prefetch( const ResourcePtr & _resource, const PrefetcherObserverInterfacePtr & _observer ) const
    {
        Content * content = _resource->getContent();

        MENGINE_ASSERTION_MEMORY_PANIC( content, false, "resource '%s' is not contentable"
            , _resource->getType().c_str()
        );

        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();
        const FilePath & filePath = content->getFilePath();

        uint32_t magicNumber = _resource->getMagicNumber();
        uint32_t magicVersion = _resource->getMagicVersion();

        if( PREFETCHER_SERVICE()
            ->prefetchStream( fileGroup, filePath, m_archivator, magicNumber, magicVersion, _observer ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////        
    bool ArchiveResourcePrefetcher::_unfetch( const ResourcePtr & _resource ) const
    {
        Content * content = _resource->getContent();

        MENGINE_ASSERTION_MEMORY_PANIC( content, false, "resource '%s' is not contentable"
            , _resource->getType().c_str()
        );

        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();
        const FilePath & filePath = content->getFilePath();

        bool successful = PREFETCHER_SERVICE()
            ->unfetch( fileGroup, filePath );

        return successful;
    }
}