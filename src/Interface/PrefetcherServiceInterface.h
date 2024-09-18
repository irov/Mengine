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
#include "Interface/ContentInterface.h"

#include "Kernel/IntrusivePtr.h"
#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/Visitor.h"
#include "Kernel/ThreadTask.h"
#include "Kernel/LambdaPrefetchTask.h"

#include "Config/Lambda.h"

namespace Mengine
{
    class PrefetcherServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "PrefetcherService" )

    public:
        virtual bool prefetchImageDecoder( const ContentInterfacePtr & _content, const PrefetcherObserverInterfacePtr & _observer ) = 0;
        virtual bool getImageDecoder( const ContentInterfacePtr & _content, ImageDecoderInterfacePtr * const _decoder ) = 0;

    public:
        virtual bool prefetchSoundDecoder( const ContentInterfacePtr & _content, const PrefetcherObserverInterfacePtr & _observer ) = 0;
        virtual bool getSoundDecoder( const ContentInterfacePtr & _content, SoundDecoderInterfacePtr * const _decoder ) = 0;

    public:
        virtual bool prefetchData( const ContentInterfacePtr & _content, const DataflowContext * _context, const PrefetcherObserverInterfacePtr & _observer ) = 0;
        virtual bool getData( const ContentInterfacePtr & _content, DataInterfacePtr * const _data ) = 0;
        virtual bool popData( const ContentInterfacePtr & _content, DataInterfacePtr * const _data ) = 0;

    public:
        virtual bool prefetchStream( const ContentInterfacePtr & _content, const ArchivatorInterfacePtr & _archivator, uint32_t magic_number, uint32_t magic_version, const PrefetcherObserverInterfacePtr & _observer ) = 0;
        virtual bool getStream( const ContentInterfacePtr & _content, MemoryInterfacePtr * const _data ) = 0;

    public:
        virtual bool unfetch( const ContentInterfacePtr & _content ) = 0;

    public:        
        virtual void foreachPrefetches( const LambdaPrefetchTask & _lambda ) const = 0;
        virtual void visitPrefetches( const VisitorPtr & _visitor ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define PREFETCHER_SERVICE()\
    ((Mengine::PrefetcherServiceInterface *)SERVICE_GET(Mengine::PrefetcherServiceInterface))
//////////////////////////////////////////////////////////////////////////