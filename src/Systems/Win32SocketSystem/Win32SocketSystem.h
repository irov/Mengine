#pragma once

#include "Interface/SocketSystemInterface.h"
#include "Interface/FactoryInterface.h"

#include "Environment/Windows/WindowsIncluder.h"
#include "Kernel/ServiceBase.h"

#include <winsock2.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32SocketSystem
        : public ServiceBase<SocketSystemInterface>
    {
        DECLARE_FACTORABLE( Win32SocketSystem );

    public:
        Win32SocketSystem();
        ~Win32SocketSystem() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        SocketInterfacePtr createSocket( const DocumentInterfacePtr & _doc ) override;
        DatagramInterfacePtr createDatagram( const DocumentInterfacePtr & _doc ) override;
        ESocketResult resolveAddress( const Char * _host, uint16_t _port, ESocketAddressFamily _family, SocketAddress * const _address ) const override;

    protected:
        WSADATA m_wsaData;
        FactoryInterfacePtr m_factorySocket;
        FactoryInterfacePtr m_factoryDatagram;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
