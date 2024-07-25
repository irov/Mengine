#pragma once

#include "Interface/ContentInterface.h"
#include "Interface/PrefetcherObserverInterface.h"
#include "Interface/ImageCodecInterface.h"
#include "Interface/SoundCodecInterface.h"
#include "Interface/DataInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/ThreadTaskInterface.h"

#include "Kernel/LambdaPrefetchTask.h"
#include "Kernel/Visitor.h"

namespace Mengine
{
    namespace Helper
    {
       //////////////////////////////////////////////////////////////////////////
       bool prefetchImageDecoder( const ContentInterfacePtr & _content, const PrefetcherObserverInterfacePtr & _observer );
       bool getPrefetchImageDecoder( const ContentInterfacePtr & _content, ImageDecoderInterfacePtr * const _decoder );
       //////////////////////////////////////////////////////////////////////////
       bool prefetchSoundDecoder( const ContentInterfacePtr & _content, const PrefetcherObserverInterfacePtr & _observer );
       bool getPrefetchSoundDecoder( const ContentInterfacePtr & _content, SoundDecoderInterfacePtr * const _decoder );
       //////////////////////////////////////////////////////////////////////////
       bool prefetchData( const ContentInterfacePtr & _content, const DataflowContext * _context, const PrefetcherObserverInterfacePtr & _observer );
       bool getPrefetchData( const ContentInterfacePtr & _content, DataInterfacePtr * const _data );
       bool popPrefetchData( const ContentInterfacePtr & _content, DataInterfacePtr * const _data );
       //////////////////////////////////////////////////////////////////////////
       bool prefetchStream( const ContentInterfacePtr & _content, const ArchivatorInterfacePtr & _archivator, uint32_t _magicNumber, uint32_t _magicVersion, const PrefetcherObserverInterfacePtr & _observer );
       bool getPrefetchStream( const ContentInterfacePtr & _content, MemoryInterfacePtr * const _memory );
       //////////////////////////////////////////////////////////////////////////
       bool unfetch( const ContentInterfacePtr & _content );
       //////////////////////////////////////////////////////////////////////////
       void foreachPrefetches( const LambdaPrefetchTask & _lambda );
       void visitPrefetches( const VisitorPtr & _visitor );
       //////////////////////////////////////////////////////////////////////////
    }
}
