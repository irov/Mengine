#include "Interface/PrefetcherServiceInterface.h"
#include "Interface/ThreadQueueInterface.h"

#include "ThreadTaskPrefetch.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/VectorConstString.h"
#include "Kernel/Factory.h"

#include "Kernel/Pair.h"
#include "Kernel/Map.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct PrefetchReceiver
    {
        uint32_t refcount;
        ThreadTaskPrefetchPtr prefetcher;
    };
    //////////////////////////////////////////////////////////////////////////
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
        bool prefetchImageDecoder( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _codecType, const PrefetcherObserverInterfacePtr & _observer ) override;
        bool getImageDecoder( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, ImageDecoderInterfacePtr & _decoder ) override;

    public:
        bool prefetchSoundDecoder( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _codecType, const PrefetcherObserverInterfacePtr & _observer ) override;
        bool getSoundDecoder( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, SoundDecoderInterfacePtr & _decoder ) override;

    public:
        bool prefetchData( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DataflowInterfacePtr & _dataflow, const PrefetcherObserverInterfacePtr & _observer ) override;
        bool getData( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, DataInterfacePtr & _data ) override;
        bool popData( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, DataInterfacePtr & _data ) override;

    public:
        bool prefetchStream( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ArchivatorInterfacePtr & _archivator, uint32_t _magicNumber, uint32_t _magicVersion, const PrefetcherObserverInterfacePtr & _observer ) override;
        bool getStream( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, MemoryInterfacePtr & _memory ) override;

    public:
        bool unfetch( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath ) override;

    public:
        void visitPrefetches( const VisitorPtr & _visitor ) const override;

    protected:
        bool hasPrefetch_( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, PrefetchReceiver ** _receiver ) const;
        bool getPrefetch_( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, PrefetchReceiver ** _receiver ) const;
        bool popPrefetch_( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, ThreadTaskPrefetchPtr * _prefetch );

    protected:
        ThreadQueueInterfacePtr m_threadQueue;

        typedef VectorConstString VectorThreads;
        VectorThreads m_threads;

        FactoryPtr m_factoryThreadTaskPrefetchImageDecoder;
        FactoryPtr m_factoryThreadTaskPrefetchSoundDecoder;
        FactoryPtr m_factoryThreadTaskPrefetchDataflow;
        FactoryPtr m_factoryThreadTaskPrefetchStream;

        typedef Pair<ConstString, FilePath> KeyPrefetchReceiver;
        typedef Map<KeyPrefetchReceiver, PrefetchReceiver> MapPrefetchReceiver;
        MapPrefetchReceiver m_prefetchReceiver;
    };
    //////////////////////////////////////////////////////////////////////////
}