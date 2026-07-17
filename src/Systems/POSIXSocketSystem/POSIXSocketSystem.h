#pragma once

#include "Interface/SocketSystemInterface.h"
#include "Interface/FactoryInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class POSIXSocketSystem
        : public ServiceBase<SocketSystemInterface>
    {
        DECLARE_FACTORABLE( POSIXSocketSystem );

    public:
        POSIXSocketSystem();
        ~POSIXSocketSystem() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        SocketInterfacePtr createSocket( const DocumentInterfacePtr & _doc ) override;
        DatagramInterfacePtr createDatagram( const DocumentInterfacePtr & _doc ) override;
        ESocketResult resolveAddress( const Char * _host, uint16_t _port, ESocketAddressFamily _family, SocketAddress * const _address ) const override;

    protected:
        FactoryInterfacePtr m_factorySocket;
        FactoryInterfacePtr m_factoryDatagram;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
