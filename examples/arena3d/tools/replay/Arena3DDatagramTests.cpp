#include "POSIXDatagram.h"

#include "Kernel/Array.h"

#include "Config/StdIO.h"
#include "Config/StdString.h"

int main()
{
    Mengine::POSIXDatagram sender;
    Mengine::POSIXDatagram receiver;
    sender.setUniqueIdentity( 1 );
    receiver.setUniqueIdentity( 2 );
    if( sender.open( Mengine::ESocketAddressFamily::IPv4 ) != Mengine::ESocketResult::Successful )
    {
        return 1;
    }
    if( receiver.open( Mengine::ESocketAddressFamily::IPv4 ) != Mengine::ESocketResult::Successful )
    {
        return 2;
    }
    Mengine::SocketAddress address;
    address.family = Mengine::ESocketAddressFamily::IPv4;
    address.address[0] = 127;
    address.address[3] = 1;
    if( receiver.bind( address ) != Mengine::ESocketResult::Successful )
    {
        return 3;
    }
    if( receiver.getLocalAddress( &address ) != Mengine::ESocketResult::Successful )
    {
        return 4;
    }
    Mengine::Array<uint8_t, 32> payload{};
    payload[0] = 19;
    payload[4] = 73;
    Mengine::Array<uint8_t, 16> received{};
    const void * bytes = payload.data();
    void * buffer = received.data();
    size_t capacity = received.size();
    size_t sent = 0;
    size_t size = 0;
    Mengine::SocketAddress source;

    // Intact datagrams must not be reported as truncated on Darwin.
    if( sender.sendTo( address, bytes, 5, &sent ) != Mengine::ESocketResult::Successful )
    {
        return 5;
    }
    if( receiver.waitReadable( 1000 ) != Mengine::ESocketResult::Successful )
    {
        return 6;
    }
    Mengine::ESocketResult status = receiver.receiveFrom( buffer, capacity, &size, &source );
    if( status != Mengine::ESocketResult::Successful || size != 5 )
    {
        Mengine::StdIO::fprintf( stderr, "Intact UDP datagram returned status=%u size=%zu\n", static_cast<unsigned int>( status ), size );
        return 7;
    }
    if( Mengine::StdString::memcmp( buffer, bytes, size ) != 0 )
    {
        return 8;
    }
    size_t payloadSize = payload.size();
    if( sender.sendTo( address, bytes, payloadSize, &sent ) != Mengine::ESocketResult::Successful )
    {
        return 9;
    }
    if( receiver.waitReadable( 1000 ) != Mengine::ESocketResult::Successful )
    {
        return 10;
    }
    if( receiver.receiveFrom( buffer, capacity, &size, &source ) != Mengine::ESocketResult::Truncated )
    {
        return 11;
    }
    if( size != capacity )
    {
        return 12;
    }
    // Reading an oversized datagram must not poison the following empty packet.
    if( sender.sendTo( address, bytes, 0, &sent ) != Mengine::ESocketResult::Successful )
    {
        return 13;
    }
    if( receiver.waitReadable( 1000 ) != Mengine::ESocketResult::Successful )
    {
        return 14;
    }
    if( receiver.receiveFrom( buffer, capacity, &size, &source ) != Mengine::ESocketResult::Successful )
    {
        return 15;
    }
    if( size != 0 )
    {
        return 16;
    }
    Mengine::StdIO::printf( "Native UDP: intact, oversized and empty datagrams passed\n" );

    return 0;
}
