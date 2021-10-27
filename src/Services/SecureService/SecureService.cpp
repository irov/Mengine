#include "SecureService.h"

#include "Interface/OptionsServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/HashHelper.h"

#include "Config/StdString.h"

#ifndef MENGINE_SECURE_VALUE
#define MENGINE_SECURE_VALUE "0123456789A"
#endif

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
        const Char * secure = GET_OPTION_VALUE( "secure", MENGINE_SECURE_VALUE );

        size_t secure_len = MENGINE_STRLEN( secure );

        m_secureHash = Helper::makeHash( secure, secure_len );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SecureService::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    HashType SecureService::getSecureHash() const
    {
        return m_secureHash;
    }
    //////////////////////////////////////////////////////////////////////////
    void SecureService::protectData( void * const _buffer, size_t _size ) const
    {
        size_t tail = _size % 8;
        size_t head = _size - tail;

        for( size_t index = 0; index != head; index += 8 )
        {
            *(int64_t *)_buffer ^= m_secureHash;
        }

        union
        {
            int8_t tail_buffer[8];
            int64_t tail64;
        };

        MENGINE_MEMCPY( tail_buffer, (int8_t *)_buffer + head, tail );

        tail64 ^= m_secureHash;

        MENGINE_MEMCPY( (int8_t *)_buffer + head, tail_buffer, tail );
    }
    //////////////////////////////////////////////////////////////////////////
    void SecureService::unprotectData( void * const _buffer, size_t _size ) const
    {
        size_t tail = _size % 8;
        size_t head = _size - tail;

        for( size_t index = 0; index != head; index += 8 )
        {
            *(int64_t *)_buffer ^= m_secureHash;
        }

        union
        {
            int8_t tail_buffer[8];
            int64_t tail64;
        };

        MENGINE_MEMCPY( tail_buffer, (int8_t *)_buffer + head, tail );

        tail64 ^= m_secureHash;

        MENGINE_MEMCPY( (int8_t *)_buffer + head, tail_buffer, tail );
    }
    //////////////////////////////////////////////////////////////////////////
}
