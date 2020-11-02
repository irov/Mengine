#include "GOAPService.h"

#include "Kernel/MemoryAllocator.h"
#include "Kernel/AllocatorHelper.h"

#include "GOAP/GOAP.h"

#include "GOAP/SourceProviderInterface.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( GOAPService, Mengine::GOAPService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MengineGOAPAllocator
        : public GOAP::Allocator
    {
    public:
        MengineGOAPAllocator()
        {
        }

        ~MengineGOAPAllocator() override
        {
        }

    protected:
        void * allocate( size_t _size ) override
        {
            void * p = Helper::allocateMemory( _size, "goap" );

            return p;
        }

        void deallocate( void * _ptr ) override
        {
            Helper::deallocateMemory( _ptr, "goap" );
        }
    };
    //////////////////////////////////////////////////////////////////////////
    GOAPService::GOAPService()
        : m_allocator( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    GOAPService::~GOAPService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool GOAPService::_initializeService()
    {
        m_allocator = Helper::newT<MengineGOAPAllocator>();

        m_kernel = GOAP::Helper::makeKernel( m_allocator );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GOAPService::_finalizeService()
    {
        m_kernel = nullptr;

        Helper::deleteT( static_cast<MengineGOAPAllocator *>(m_allocator) );
        m_allocator = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    GOAP::SourceInterfacePtr GOAPService::makeSource()
    {
        GOAP::SourceInterfacePtr source = m_kernel->makeSource();

        return source;
    }
    //////////////////////////////////////////////////////////////////////////
    GOAP::ChainInterfacePtr GOAPService::makeChain( const GOAP::SourceInterfacePtr & _source, const LambdaSourceCallback & _callback, const Char * _file, uint32_t _line )
    {
        GOAP::ChainInterfacePtr chain = m_kernel->makeChain( _source, _file, _line );

        if( _callback != nullptr )
        {
            GOAP::ChainProviderInterfacePtr provider = GOAP::Helper::makeChainProvider( m_allocator, _callback );

            chain->setCallbackProvider( provider );
        }

        return chain;
    }
    //////////////////////////////////////////////////////////////////////////
    GOAP::EventInterfacePtr GOAPService::makeEvent()
    {
        GOAP::EventInterfacePtr event = m_kernel->makeEvent();

        return event;
    }
    //////////////////////////////////////////////////////////////////////////
    GOAP::SemaphoreInterfacePtr GOAPService::makeSemaphore( int32_t _value )
    {
        GOAP::EventInterfacePtr event = m_kernel->makeEvent();

        GOAP::SemaphoreInterfacePtr semaphore = m_kernel->makeSemaphore( event, _value );

        return semaphore;
    }
    //////////////////////////////////////////////////////////////////////////
    GOAP::TimerInterfacePtr GOAPService::makeTimer()
    {
        GOAP::TimerInterfacePtr timer = m_kernel->makeTimer();

        return timer;
    }
}