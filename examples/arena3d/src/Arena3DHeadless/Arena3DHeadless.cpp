#include "Arena3DHeadless.h"

#include "Interface/ServiceProviderInterface.h"
#include "Interface/SocketSystemInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/TimeSystemInterface.h"

#include "Kernel/DocumentHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_EXTERN( ServiceProvider );
//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AllocatorSystem );
SERVICE_EXTERN( EnumeratorService );
SERVICE_EXTERN( StringizeService );
SERVICE_EXTERN( DocumentService );
SERVICE_EXTERN( TimeSystem );
SERVICE_EXTERN( ThreadSystem );
SERVICE_EXTERN( StatisticService );
SERVICE_EXTERN( SocketSystem );
SERVICE_EXTERN( SocketService );
//////////////////////////////////////////////////////////////////////////
namespace Arena3D
{
    //////////////////////////////////////////////////////////////////////////
    bool HeadlessEngine::initialize()
    {
        if( SERVICE_PROVIDER_CREATE( ServiceProvider, &m_serviceProvider ) == false )
        {
            return false;
        }
        SERVICE_PROVIDER_SETUP( m_serviceProvider );

        if( SERVICE_CREATE( AllocatorSystem, nullptr ) == false ||
            SERVICE_CREATE( EnumeratorService, nullptr ) == false ||
            SERVICE_CREATE( StringizeService, nullptr ) == false ||
            SERVICE_CREATE( DocumentService, nullptr ) == false ||
            SERVICE_CREATE( TimeSystem, MENGINE_DOCUMENT_FUNCTION ) == false ||
            SERVICE_CREATE( ThreadSystem, MENGINE_DOCUMENT_FUNCTION ) == false ||
            SERVICE_CREATE( StatisticService, MENGINE_DOCUMENT_FUNCTION ) == false ||
            SERVICE_CREATE( SocketSystem, MENGINE_DOCUMENT_FUNCTION ) == false ||
            SERVICE_CREATE( SocketService, MENGINE_DOCUMENT_FUNCTION ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void HeadlessEngine::finalize()
    {
        if( m_serviceProvider == nullptr )
        {
            return;
        }

        SERVICE_FINALIZE( SocketService );
        SERVICE_DESTROY( SocketService );
        SERVICE_FINALIZE( SocketSystem );
        SERVICE_DESTROY( SocketSystem );
        SERVICE_FINALIZE( StatisticService );
        SERVICE_DESTROY( StatisticService );
        SERVICE_FINALIZE( ThreadSystem );
        SERVICE_DESTROY( ThreadSystem );
        SERVICE_FINALIZE( TimeSystem );
        SERVICE_DESTROY( TimeSystem );
        SERVICE_FINALIZE( DocumentService );
        SERVICE_DESTROY( DocumentService );
        SERVICE_FINALIZE( StringizeService );
        SERVICE_DESTROY( StringizeService );
        SERVICE_FINALIZE( EnumeratorService );
        SERVICE_DESTROY( EnumeratorService );
        SERVICE_FINALIZE( AllocatorSystem );
        SERVICE_DESTROY( AllocatorSystem );

        SERVICE_PROVIDER_FINALIZE( m_serviceProvider );
        SERVICE_PROVIDER_DESTROY( m_serviceProvider );
        m_serviceProvider = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HeadlessEngine::resolve( const char * host, uint16_t port, Mengine::SocketAddress * address ) const
    {
        return SOCKET_SYSTEM()->resolveAddress( host, port, Mengine::ESocketAddressFamily::IPv4, address ) == Mengine::ESocketResult::Successful;
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t HeadlessEngine::getTimeMilliseconds() const
    {
        return TIME_SYSTEM()->getPlatformTimestamp();
    }
    //////////////////////////////////////////////////////////////////////////
    void HeadlessEngine::sleep( uint64_t milliseconds ) const
    {
        THREAD_SYSTEM()->sleep( milliseconds );
    }
    //////////////////////////////////////////////////////////////////////////
}
