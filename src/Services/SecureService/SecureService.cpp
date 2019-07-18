#include "SecureService.h"

#include "Kernel/Logger.h"
#include "Kernel/HashHelper.h"

#include <string.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SecureService, Mengine::SecureService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SecureService::SecureService()
        : m_secureHash( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SecureService::~SecureService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SecureService::_initializeService()
    {
        Mengine::HashType secureHash =
#include "Config/Secure.h"
            ;

        m_secureHash = secureHash;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SecureService::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    HashType SecureService::getSequreHash() const
    {
        return m_secureHash;
    }
    //////////////////////////////////////////////////////////////////////////
    void SecureService::protectData( void * _buffer, size_t _size ) const
    {
        size_t tail = _size % 8;
        size_t head = _size - tail;

        for( size_t index = 0; index != head; index += 8 )
        {
            *(int64_t *)_buffer ^= m_secureHash;
        }

        union
        {
            int8_t tail_buffer[8] = { 0 };
            int64_t tail64;
        };
        
        ::memcpy( tail_buffer, (int8_t *)_buffer + head, tail );

        tail64 ^= m_secureHash;

        ::memcpy( (int8_t *)_buffer + head, tail_buffer, tail );
    }
    //////////////////////////////////////////////////////////////////////////
    void SecureService::unprotectData( void * _buffer, size_t _size ) const
    {
        size_t tail = _size % 8;
        size_t head = _size - tail;

        for( size_t index = 0; index != head; index += 8 )
        {
            *(int64_t *)_buffer ^= m_secureHash;
        }

        union
        {
            int8_t tail_buffer[8] = { 0 };
            int64_t tail64;
        };

        ::memcpy( tail_buffer, (int8_t *)_buffer + head, tail );

        tail64 ^= m_secureHash;

        ::memcpy( (int8_t *)_buffer + head, tail_buffer, tail );
    }
}
