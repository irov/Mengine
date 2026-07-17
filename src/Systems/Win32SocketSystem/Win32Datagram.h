#pragma once

#include "Interface/DatagramInterface.h"

#include "Environment/Windows/WindowsIncluder.h"
#include "Kernel/Factorable.h"

#include <winsock2.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32Datagram
        : public DatagramInterface
    {
        DECLARE_FACTORABLE( Win32Datagram );

    public:
        Win32Datagram();
        ~Win32Datagram() override;

    public:
        ESocketResult open( ESocketAddressFamily _family ) override;
        ESocketResult bind( const SocketAddress & _address ) override;
        ESocketResult connect( const SocketAddress & _address ) override;
        ESocketResult setBlocking( bool _blocking ) override;
        ESocketResult setReuseAddress( bool _reuse ) override;
        ESocketResult setSendBufferSize( uint32_t _size ) override;
        ESocketResult setReceiveBufferSize( uint32_t _size ) override;
        ESocketResult sendTo( const SocketAddress & _address, const void * _data, size_t _size, size_t * const _sent ) override;
        ESocketResult send( const void * _data, size_t _size, size_t * const _sent ) override;
        ESocketResult receiveFrom( void * const _data, size_t _capacity, size_t * const _received, SocketAddress * const _address ) override;
        ESocketResult receive( void * const _data, size_t _capacity, size_t * const _received ) override;
        ESocketResult waitReadable( uint32_t _timeoutMs ) override;
        ESocketResult waitWritable( uint32_t _timeoutMs ) override;
        ESocketResult getLocalAddress( SocketAddress * const _address ) const override;
        void close() override;

    protected:
        ESocketResult wait_( bool _read, uint32_t _timeoutMs );
        ESocketResult errorResult_();

    protected:
        SOCKET m_socket;
        ESocketAddressFamily m_family;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32Datagram> Win32DatagramPtr;
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
