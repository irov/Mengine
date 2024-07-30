#include "POSIXUnicodeSystem.h"

#include "Interface/ThreadSystemInterface.h"

#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ThreadMutexScope.h"

#include "Config/StdString.h"
#include "Config/Algorithm.h"

#include <pthread.h>
#include <iconv.h>
#include <unistd.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( UnicodeSystem, Mengine::POSIXUnicodeSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    constexpr const Char * SDL_UCS_wchar_t = sizeof( WChar ) == 2 ? "UCS-2-INTERNAL" : "UCS-4-INTERNAL";
    //////////////////////////////////////////////////////////////////////////
    POSIXUnicodeSystem::POSIXUnicodeSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    POSIXUnicodeSystem::~POSIXUnicodeSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const ServiceRequiredList & POSIXUnicodeSystem::requiredServices() const
    {
        static ServiceRequiredList required = {
            SERVICE_ID( ThreadSystemInterface )
        };

        return required;
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXUnicodeSystem::_initializeService()
    {
        ThreadMutexInterfacePtr mutex = THREAD_SYSTEM()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex );

        m_mutex = mutex;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXUnicodeSystem::_finalizeService()
    {
        m_mutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXUnicodeSystem::unicodeToUtf8( const WChar * _unicode, size_t _unicodeSize, Char * const _utf8, size_t _utf8Capacity, size_t * const _utf8Size )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        size_t unicodeSize = (_unicodeSize == (size_t)-1) ? MENGINE_WCSLEN( _unicode ) + 1 : _unicodeSize + 1;

        size_t utf8Size = 0;
        for( size_t i = 0; i != unicodeSize; ++i )
        {
            WChar wc = _unicode[i];

            if( wc < 0x80 )
            {
                if( _utf8 != nullptr && utf8Size < _utf8Capacity )
                {
                    _utf8[utf8Size] = (Char)wc;
                }

                utf8Size += 1;
            }
            else if( wc < 0x800 )
            {
                if( _utf8 != nullptr && utf8Size + 1 < _utf8Capacity )
                {
                    _utf8[utf8Size] = (Char)(0xC0 | (wc >> 6));
                    _utf8[utf8Size + 1] = (Char)(0x80 | (wc & 0x3F));
                }

                utf8Size += 2;
            }
            else if( wc < 0x10000 )
            {
                if( _utf8 != nullptr && utf8Size + 2 < _utf8Capacity )
                {
                    _utf8[utf8Size] = (Char)(0xE0 | (wc >> 12));
                    _utf8[utf8Size + 1] = (Char)(0x80 | ((wc >> 6) & 0x3F));
                    _utf8[utf8Size + 2] = (Char)(0x80 | (wc & 0x3F));
                }

                utf8Size += 3;
            }
            else if( wc < 0x200000 )
            {
                if( _utf8 != nullptr && utf8Size + 3 < _utf8Capacity )
                {
                    _utf8[utf8Size] = (Char)(0xF0 | (wc >> 18));
                    _utf8[utf8Size + 1] = (Char)(0x80 | ((wc >> 12) & 0x3F));
                    _utf8[utf8Size + 2] = (Char)(0x80 | ((wc >> 6) & 0x3F));
                    _utf8[utf8Size + 3] = (Char)(0x80 | (wc & 0x3F));
                }

                utf8Size += 4;
            }
            else
            {
                return false;
            }
        }

        if( _utf8 != nullptr )
        {
            if( utf8Size >= _utf8Capacity )
            {
                return false;
            }

            _utf8[utf8Size] = '\0';
        }

        if( _utf8Size )
        {
            *_utf8Size = utf8Size;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXUnicodeSystem::utf8ToUnicode( const Char * _utf8, size_t _utf8Size, WChar * const _unicode, size_t _unicodeCapacity, size_t * const _sizeUnicode )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        iconv_t cd = ::iconv_open( SDL_UCS_wchar_t, "UTF-8" );

        if( cd == (iconv_t)-1 )
        {
            return false;
        }

        size_t utf8Size = (_utf8Size == MENGINE_UNKNOWN_SIZE) ? MENGINE_STRLEN( _utf8 ) + 1 : _utf8Size + 1;

        const char * iconv_inbuf = (const char *)_utf8;
        size_t iconv_insize = utf8Size * sizeof( Char );

        char * iconv_outbuf = (char *)_unicode;
        size_t iconv_outsize = _unicodeCapacity * sizeof( WChar );

        if( _unicode == nullptr )
        {
            m_unicodeBuffer[0] = L'\0';

            iconv_outbuf = (char *)m_unicodeBuffer;
            iconv_outsize = MENGINE_UNICODE_CONVERT_BUFFER * sizeof( WChar );
        }

        size_t iconv_capacity = iconv_outsize;

        size_t code = ::iconv( cd, &iconv_inbuf, &iconv_insize, &iconv_outbuf, &iconv_outsize );

        ::iconv_close( cd );

        if( code == (size_t)-1 )
        {
            switch( errno )
            {
            case E2BIG:
                {
                    MENGINE_ASSERTION_FATAL( _utf8 == nullptr, "utf8 override buffer [%u]", MENGINE_UNICODE_CONVERT_BUFFER );

                    return false;
                } break;
            case EILSEQ:
                {
                    return false;
                } break;
            case EINVAL:
                {
                    return false;
                } break;
            default:
                {
                    return false;
                } break;
            }
        }

        size_t unicodeSize = (iconv_capacity - iconv_outsize) / sizeof( WChar ) - 1;

        if( _sizeUnicode != nullptr )
        {
            *_sizeUnicode = unicodeSize;
        }

        if( _unicode != nullptr )
        {
            if( _unicodeCapacity < unicodeSize )
            {
                return false;
            }

            _unicode[unicodeSize] = L'\0';
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
