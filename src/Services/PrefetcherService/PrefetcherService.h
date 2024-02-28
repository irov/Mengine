#include "Interface/PrefetcherServiceInterface.h"
#include "Interface/ThreadQueueInterface.h"
#include "Interface/FactoryInterface.h"

#include "ThreadTaskPrefetch.h"
#include "PrefetchReceiver.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/VectorConstString.h"
#include "Kernel/Hashtable2.h"

namespace Mengine
{    
    class PrefetcherService
        : public ServiceBase<PrefetcherServiceInterface>
    {
    public:
        PrefetcherService();
        ~PrefetcherService() override;

    public:
        bool _availableService() const override;
        bool _initializeService() override;
        void _finalizeService() override;
        void _stopService() override;

    public:
        bool prefetchImageDecoder( const ContentInterfacePtr & _content, const PrefetcherObserverInterfacePtr & _observer ) override;
        bool getImageDecoder( const ContentInterfacePtr & _content, ImageDecoderInterfacePtr * const _decoder ) override;

    public:
        bool prefetchSoundDecoder( const ContentInterfacePtr & _content, const PrefetcherObserverInterfacePtr & _observer ) override;
        bool getSoundDecoder( const ContentInterfacePtr & _content, SoundDecoderInterfacePtr * const _decoder ) override;

    public:
        bool prefetchData( const ContentInterfacePtr & _content, const DataflowContext * _context, const PrefetcherObserverInterfacePtr & _observer ) override;
        bool getData( const ContentInterfacePtr & _content, DataInterfacePtr * const _data ) override;
        bool popData( const ContentInterfacePtr & _content, DataInterfacePtr * const _data ) override;

    public:
        bool prefetchStream( const ContentInterfacePtr & _content, const ArchivatorInterfacePtr & _archivator, uint32_t _magicNumber, uint32_t _magicVersion, const PrefetcherObserverInterfacePtr & _observer ) override;
        bool getStream( const ContentInterfacePtr & _content, MemoryInterfacePtr * const _memory ) override;

    public:
        bool unfetch( const ContentInterfacePtr & _content ) override;

    public:
        void foreachPrefetches( const LambdaPrefetchTask & _lambda ) const override;
        void visitPrefetches( const VisitorPtr & _visitor ) const override;

    protected:
        bool hasPrefetch_( const ContentInterfacePtr & _content, PrefetchReceiverPtr * const _receiver ) const;
        bool getPrefetch_( const ContentInterfacePtr & _content, PrefetchReceiverPtr * const _receiver ) const;
        bool popPrefetch_( const ContentInterfacePtr & _content, ThreadTaskPrefetchPtr * const _prefetch );

    protected:
        FactoryInterfacePtr m_factoryPrefetchReceiver;

        ThreadQueueInterfacePtr m_threadQueue;

        typedef VectorConstString VectorThreads;
        VectorThreads m_threads;

        FactoryInterfacePtr m_factoryThreadTaskPrefetchImageDecoder;
        FactoryInterfacePtr m_factoryThreadTaskPrefetchSoundDecoder;
        FactoryInterfacePtr m_factoryThreadTaskPrefetchDataflow;
        FactoryInterfacePtr m_factoryThreadTaskPrefetchStream;

        typedef Hashtable2<FileGroupInterfacePtr, FilePath, PrefetchReceiverPtr> HashtablePrefetchReceiver;
        HashtablePrefetchReceiver m_prefetchReceivers;
    };    
}