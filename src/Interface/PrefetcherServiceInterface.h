#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/ImageCodecInterface.h"
#include "Interface/SoundCodecInterface.h"
#include "Interface/PrefetcherObserverInterface.h"
#include "Interface/DataInterface.h"
#include "Interface/DataflowInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/IntrusivePtr.h"
#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/Visitor.h"

namespace Mengine
{
    class PrefetcherServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "PrefetcherService" )

    public:
        virtual void update() = 0;

    public:
        virtual bool prefetchImageDecoder( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _codecType, const PrefetcherObserverInterfacePtr & _observer ) = 0;
        virtual bool getImageDecoder( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, ImageDecoderInterfacePtr & _decoder ) = 0;

    public:
        virtual bool prefetchSoundDecoder( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _codecType, const PrefetcherObserverInterfacePtr & _observer ) = 0;
        virtual bool getSoundDecoder( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, SoundDecoderInterfacePtr & _decoder ) = 0;

    public:
        virtual bool prefetchData( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DataflowInterfacePtr & _dataflow, const PrefetcherObserverInterfacePtr & _observer ) = 0;
        virtual bool getData( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, DataInterfacePtr & _data ) = 0;
        virtual bool popData( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, DataInterfacePtr & _data ) = 0;

    public:
        virtual bool prefetchStream( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ArchivatorInterfacePtr & _archivator, uint32_t magic_number, uint32_t magic_version, const PrefetcherObserverInterfacePtr & _observer ) = 0;
        virtual bool getStream( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, MemoryInterfacePtr & _data ) = 0;

    public:
        virtual bool unfetch( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath ) = 0;

    public:
        virtual void visitPrefetches( const VisitorPtr & _visitor ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
#define PREFETCHER_SERVICE()\
	((Mengine::PrefetcherServiceInterface*)SERVICE_GET(Mengine::PrefetcherServiceInterface))
    //////////////////////////////////////////////////////////////////////////
}

