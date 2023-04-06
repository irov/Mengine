#include "SDLUnicodeSystem.h"

#include "Interface/ThreadSystemInterface.h"

#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ThreadMutexScope.h"

#include "Config/StdString.h"
#include "Config/Algorithm.h"

#include "SDL_stdinc.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( UnicodeSystem, Mengine::SDLUnicodeSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////    
#if defined(MENGINE_PLATFORM_LINUX)
    //////////////////////////////////////////////////////////////////////////
    constexpr const Char * SDL_UCS_wchar_t = sizeof( WChar ) == 2 ? "UCS-2" : "UCS-4";
    //////////////////////////////////////////////////////////////////////////
#else
    //////////////////////////////////////////////////////////////////////////
    constexpr const Char * SDL_UCS_wchar_t = sizeof( WChar ) == 2 ? "UCS-2-INTERNAL" : "UCS-4-INTERNAL";
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
    SDLUnicodeSystem::SDLUnicodeSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLUnicodeSystem::~SDLUnicodeSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const ServiceRequiredList & SDLUnicodeSystem::requiredServices() const
    {
        static ServiceRequiredList required = {
            ThreadSystemInterface::getStaticServiceID()
        };

        return required;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLUnicodeSystem::_initializeService()
    {
        ThreadMutexInterfacePtr mutex = THREAD_SYSTEM()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex );

        m_mutex = mutex;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLUnicodeSystem::_finalizeService()
    {
        m_mutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLUnicodeSystem::unicodeToUtf8( const WChar * _unicode, size_t _unicodeSize, Char * const _utf8, size_t _utf8Capacity, size_t * const _utf8Size )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        SDL_iconv_t cd = SDL_iconv_open( "UTF-8", SDL_UCS_wchar_t );

        if( cd == (SDL_iconv_t)-1 )
        {
            return false;
        }

        size_t unicodeSize = (_unicodeSize == MENGINE_UNKNOWN_SIZE) ? MENGINE_WCSLEN( _unicode ) + 1 : _unicodeSize + 1;

        const char * iconv_inbuf = (const char *)_unicode;
        size_t iconv_insize = unicodeSize * sizeof( WChar );

        char * iconv_outbuf = (char *)_utf8;
        size_t iconv_outsize = _utf8Capacity * sizeof( Char );

        if( _utf8 == nullptr )
        {
            m_utf8Buffer[0] = '\0';
            
            iconv_outbuf = (char *)m_utf8Buffer;
            iconv_outsize = MENGINE_UNICODE_CONVERT_BUFFER * sizeof( Char );
        }
        
        size_t iconv_capacity = iconv_outsize;

        size_t code = SDL_iconv( cd, &iconv_inbuf, &iconv_insize, &iconv_outbuf, &iconv_outsize );

        SDL_iconv_close( cd );

        switch( code )
        {
        case SDL_ICONV_E2BIG:
            {
                MENGINE_ASSERTION_FATAL( _utf8 == nullptr, "utf8 override buffer [%u]", MENGINE_UNICODE_CONVERT_BUFFER );

                return false;
            }break;
        case SDL_ICONV_EILSEQ:
            {
                return false;
            }break;
        case SDL_ICONV_EINVAL:
            {
                return false;
            }break;
        case SDL_ICONV_ERROR:
            {
                return false;
            }break;
        }

        size_t utf8Size = (iconv_capacity - iconv_outsize) / sizeof(Char) - 1;

        if( _utf8Size != nullptr )
        {
            *_utf8Size = utf8Size;
        }

        if( _utf8 != nullptr )
        {
            if( _utf8Capacity < utf8Size )
            {
                return false;
            }

            _utf8[utf8Size] = '\0';
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLUnicodeSystem::utf8ToUnicode( const Char * _utf8, size_t _utf8Size, WChar * const _unicode, size_t _unicodeCapacity, size_t * const _sizeUnicode )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        SDL_iconv_t cd = SDL_iconv_open( SDL_UCS_wchar_t, "UTF-8" );

        if( cd == (SDL_iconv_t)-1 )
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

        size_t code = SDL_iconv( cd, &iconv_inbuf, &iconv_insize, &iconv_outbuf, &iconv_outsize );

        SDL_iconv_close( cd );

        switch( code )
        {
        case SDL_ICONV_E2BIG:
            {
                MENGINE_ASSERTION_FATAL( _utf8 == nullptr, "utf8 override buffer [%u]", MENGINE_UNICODE_CONVERT_BUFFER );

                return false;
            }break;
        case SDL_ICONV_EILSEQ:
            {
                return false;
            }break;
        case SDL_ICONV_EINVAL:
            {
                return false;
            }break;
        case SDL_ICONV_ERROR:
            {
                return false;
            }break;
        }

        size_t unicodeSize = (iconv_capacity - iconv_outsize) / sizeof(WChar) - 1;

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
