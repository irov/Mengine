#pragma once

#include "Interface/ServantInterface.h"

#include "Config/Char.h"
#include "Config/StdDef.h"
#include "Config/StdInt.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum class ESocketAddressFamily : uint8_t
    {
        Unknown,
        IPv4,
        IPv6
    };
    //////////////////////////////////////////////////////////////////////////
    enum class ESocketProtocol : uint8_t
    {
        TCP,
        UDP
    };
    //////////////////////////////////////////////////////////////////////////
    enum class ESocketResult : uint8_t
    {
        Successful,
        WouldBlock,
        Timeout,
        Truncated,
        AddressError,
        ConnectionReset,
        Failure
    };
    //////////////////////////////////////////////////////////////////////////
    struct SocketAddress
    {
        ESocketAddressFamily family = ESocketAddressFamily::Unknown;
        uint16_t port = 0;
        uint8_t address[16] = {0};
    };
    //////////////////////////////////////////////////////////////////////////
    class DatagramInterface
        : public ServantInterface
    {
    public:
        virtual ESocketResult open( ESocketAddressFamily _family ) = 0;
        virtual ESocketResult bind( const SocketAddress & _address ) = 0;
        virtual ESocketResult connect( const SocketAddress & _address ) = 0;
        virtual ESocketResult setBlocking( bool _blocking ) = 0;
        virtual ESocketResult setReuseAddress( bool _reuse ) = 0;
        virtual ESocketResult setSendBufferSize( uint32_t _size ) = 0;
        virtual ESocketResult setReceiveBufferSize( uint32_t _size ) = 0;

    public:
        virtual ESocketResult sendTo( const SocketAddress & _address, const void * _data, size_t _size, size_t * const _sent ) = 0;
        virtual ESocketResult send( const void * _data, size_t _size, size_t * const _sent ) = 0;
        virtual ESocketResult receiveFrom( void * const _data, size_t _capacity, size_t * const _received, SocketAddress * const _address ) = 0;
        virtual ESocketResult receive( void * const _data, size_t _capacity, size_t * const _received ) = 0;

    public:
        virtual ESocketResult waitReadable( uint32_t _timeoutMs ) = 0;
        virtual ESocketResult waitWritable( uint32_t _timeoutMs ) = 0;
        virtual ESocketResult getLocalAddress( SocketAddress * const _address ) const = 0;
        virtual void close() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DatagramInterface> DatagramInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
