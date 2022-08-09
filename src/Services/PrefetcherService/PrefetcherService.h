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
        bool getImageDecoder( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, ImageDecoderInterfacePtr * const _decoder ) override;

    public:
        bool prefetchSoundDecoder( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _codecType, const PrefetcherObserverInterfacePtr & _observer ) override;
        bool getSoundDecoder( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, SoundDecoderInterfacePtr * const _decoder ) override;

    public:
        bool prefetchData( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DataflowInterfacePtr & _dataflow, const DataflowContext * _context, const PrefetcherObserverInterfacePtr & _observer ) override;
        bool getData( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, DataInterfacePtr * const _data ) override;
        bool popData( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, DataInterfacePtr * const _data ) override;

    public:
        bool prefetchStream( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ArchivatorInterfacePtr & _archivator, uint32_t _magicNumber, uint32_t _magicVersion, const PrefetcherObserverInterfacePtr & _observer ) override;
        bool getStream( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, MemoryInterfacePtr * const _memory ) override;

    public:
        bool unfetch( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath ) override;

    public:
        void visitPrefetches( const VisitorPtr & _visitor ) const override;

    protected:
        bool hasPrefetch_( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, PrefetchReceiverPtr * const _receiver ) const;
        bool getPrefetch_( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, PrefetchReceiverPtr * const _receiver ) const;
        bool popPrefetch_( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, ThreadTaskPrefetchPtr * const _prefetch );

    protected:
        FactoryInterfacePtr m_factoryPrefetchReceiver;

        ThreadQueueInterfacePtr m_threadQueue;

        typedef VectorConstString VectorThreads;
        VectorThreads m_threads;

        FactoryInterfacePtr m_factoryThreadTaskPrefetchImageDecoder;
        FactoryInterfacePtr m_factoryThreadTaskPrefetchSoundDecoder;
        FactoryInterfacePtr m_factoryThreadTaskPrefetchDataflow;
        FactoryInterfacePtr m_factoryThreadTaskPrefetchStream;

        typedef Hashtable2<ConstString, FilePath, PrefetchReceiverPtr> HashtablePrefetchReceiver;
        HashtablePrefetchReceiver m_prefetchReceivers;
    };
    //////////////////////////////////////////////////////////////////////////
}