#include "GOAPService.h"
#include "GOAPAllocator.h"

#include "Kernel/MemoryAllocator.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( GOAPService, Mengine::GOAPService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
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
        m_allocator = Helper::newT<GOAPAllocator>();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GOAPService::_finalizeService()
    {
        Helper::deleteT( static_cast<GOAPAllocator *>(m_allocator) );
        m_allocator = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    GOAP::SourceInterfacePtr GOAPService::makeSource()
    {
        GOAP::SourceInterfacePtr source = GOAP::Helper::makeSource( m_allocator );

        return source;
    }
    //////////////////////////////////////////////////////////////////////////
    GOAP::ChainInterfacePtr GOAPService::makeChain( const GOAP::SourceInterfacePtr & _source, const LambdaSourceCallback & _callback, const Char * _file, uint32_t _line )
    {
        GOAP::ChainInterfacePtr chain = GOAP::Helper::makeChain( m_allocator, _source, _file, _line );

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
        GOAP::EventInterfacePtr event = GOAP::Helper::makeEvent( m_allocator );

        return event;
    }
    //////////////////////////////////////////////////////////////////////////
    GOAP::SemaphoreInterfacePtr GOAPService::makeSemaphore( int32_t _value )
    {
        GOAP::EventInterfacePtr event = GOAP::Helper::makeEvent( m_allocator );

        GOAP::SemaphoreInterfacePtr semaphore = GOAP::Helper::makeSemaphore( m_allocator, event, _value );

        return semaphore;
    }
    //////////////////////////////////////////////////////////////////////////
    GOAP::TimerInterfacePtr GOAPService::makeTimer()
    {
        GOAP::TimerInterfacePtr timer = GOAP::Helper::makeTimer( m_allocator );

        return timer;
    }
    //////////////////////////////////////////////////////////////////////////
}