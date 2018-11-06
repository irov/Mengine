#include "Interface/PrefetcherInterface.h"
#include "Interface/ThreadQueueInterface.h"

#include "ThreadTaskPrefetchImageDecoder.h"
#include "ThreadTaskPrefetchSoundDecoder.h"
#include "ThreadTaskPrefetchDataflow.h"

#include "Kernel/ServiceBase.h"

#include "Kernel/Factory.h"

#include "Config/Map.h"

#ifndef MENGINE_PREFETCHER_THREAD_COUNT
#define MENGINE_PREFETCHER_THREAD_COUNT 2
#endif

#ifndef MENGINE_PREFETCHER_PACKET_SIZE
#define MENGINE_PREFETCHER_PACKET_SIZE 64
#endif

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
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void update() override;

    public:
        bool prefetchImageDecoder( const FileGroupInterfacePtr & _pakName, const FilePath & _fileName, const ConstString & _codec, const PrefetcherObserverInterfacePtr & _observer ) override;
        bool getImageDecoder( const FileGroupInterfacePtr & _pakName, const FilePath & _fileName, ImageDecoderInterfacePtr & _decoder ) const override;

    public:
        bool prefetchSoundDecoder( const FileGroupInterfacePtr & _pakName, const FilePath & _fileName, const ConstString & _codec, const PrefetcherObserverInterfacePtr & _observer ) override;
        bool getSoundDecoder( const FileGroupInterfacePtr& _pakName, const FilePath & _fileName, SoundDecoderInterfacePtr & _decoder ) const override;

    public:
        bool prefetchData( const FileGroupInterfacePtr& _pakName, const FilePath & _fileName, const ConstString & _dataflowType, const PrefetcherObserverInterfacePtr & _observer ) override;
        bool getData( const FileGroupInterfacePtr& _pakName, const FilePath & _fileName, DataInterfacePtr & _data ) const override;

    public:
        void unfetch( const FileGroupInterfacePtr& _pakName, const FilePath & _fileName ) override;

    public:
        void visitPrefetches( const VisitorPtr & _visitor ) const override;

    protected:
        bool hasPrefetch( const FileGroupInterfacePtr& _pakName, const FilePath & _fileName, PrefetchReceiver ** _receiver ) const;

    protected:
        ThreadQueueInterfacePtr m_threadQueue;

        FactoryPtr m_factoryThreadTaskPrefetchImageDecoder;
        FactoryPtr m_factoryThreadTaskPrefetchSoundDecoder;
        FactoryPtr m_factoryThreadTaskPrefetchDataflow;

        typedef std::pair<ConstString, FilePath> KeyPrefetchReceiver;
        typedef Map<KeyPrefetchReceiver, PrefetchReceiver> MapPrefetchReceiver;
        MapPrefetchReceiver m_prefetchReceiver;

        bool m_avaliable;
    };
	//////////////////////////////////////////////////////////////////////////
}