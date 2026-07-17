#pragma once

#include "Interface/DatagramInterface.h"
#include "Interface/ServiceProviderInterface.h"

#include "Config/StdInt.h"

namespace Arena3D
{
    //////////////////////////////////////////////////////////////////////////
    class HeadlessEngine
    {
    public:
        bool initialize();
        void finalize();

        bool resolve( const char * host, uint16_t port, Mengine::SocketAddress * address ) const;
        uint64_t getTimeMilliseconds() const;
        void sleep( uint64_t milliseconds ) const;

    private:
        Mengine::ServiceProviderInterface * m_serviceProvider = nullptr;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
