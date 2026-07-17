#pragma once

#include "Interface/SocketInterface.h"
#include "Interface/InputStreamInterface.h"
#include "Interface/OutputStreamInterface.h"

#include "Environment/Windows/WindowsIncluder.h"
#include "Kernel/Factorable.h"

#include <winsock2.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32Socket
        : public SocketInterface
        , public InputStreamInterface
        , public OutputStreamInterface
    {
        DECLARE_FACTORABLE( Win32Socket );

    public:
        Win32Socket();
        ~Win32Socket() override;

    public:
        bool connect( const SocketConnectInfo & _info ) override;
        bool bind( const SocketConnectInfo & _info, bool _blocking ) override;
        void disconnect() override;
        int32_t checkForClientConnection() override;
        bool waitForData( uint32_t _timeoutMs ) override;
        bool send( const void * _data, size_t _size, size_t * const _sent ) override;
        bool receive( void * const _data, size_t _capacity, size_t * const _received ) override;
        OutputStreamInterfacePtr getSendStream() const override;
        InputStreamInterfacePtr getReceiveStream() const override;

    public:
        size_t read( void * const _buffer, size_t _size ) override;
        bool seek( size_t _carriage ) override;
        void rewind() override;
        bool rseek( size_t _carriage ) override;
        bool skip( size_t _offset ) override;
        size_t tell() const override;
        size_t size() const override;
        bool eof() const override;
        bool time( uint64_t * const _time ) const override;
        bool memory( void ** const _memory, size_t * const _size ) override;
        size_t write( const void * _data, size_t _size ) override;
        bool flush() override;

    protected:
        SOCKET m_socket;
        bool m_blocking;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32Socket> Win32SocketPtr;
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
