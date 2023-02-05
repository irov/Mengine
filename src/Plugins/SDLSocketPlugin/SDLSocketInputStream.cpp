#include "SDLSocketInputStream.h"

#include "Kernel/AssertionNotImplemented.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDLSocketInputStream::SDLSocketInputStream()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLSocketInputStream::~SDLSocketInputStream()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDLSocketInputStream::read( void * const _buffer, size_t _size )
    {        
        TCPsocket socket = this->getSocket();

        int32_t received = SDLNet_TCP_Recv( socket, _buffer, _size );

        if( received < 0 )
        {
            return 0;
        }

        return (size_t)received;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLSocketInputStream::seek( size_t _pos )
    {
        MENGINE_UNUSED( _pos );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLSocketInputStream::rewind()
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLSocketInputStream::rseek( size_t _pos )
    {
        MENGINE_UNUSED( _pos );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLSocketInputStream::skip( size_t _size )
    {
        MENGINE_UNUSED( _size );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDLSocketInputStream::tell() const
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDLSocketInputStream::size() const
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLSocketInputStream::eof() const
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLSocketInputStream::time( uint64_t * _time ) const
    {
        MENGINE_UNUSED( _time );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLSocketInputStream::memory( void ** const _memory, size_t * const _size )
    {
        MENGINE_UNUSED( _memory );
        MENGINE_UNUSED( _size );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}