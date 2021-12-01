#include "SDLUnicodeSystem.h"

#include "SDL_stdinc.h"

#include "Kernel/Logger.h"

#include "Config/StdString.h"
#include "Config/Algorithm.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( UnicodeSystem, Mengine::SDLUnicodeSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////    
#ifdef MENGINE_PLATFORM_LINUX
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
    bool SDLUnicodeSystem::unicodeToUtf8( const WChar * _unicode, size_t _unicodeSize, Char * const _utf8, size_t _utf8Capacity, size_t * const _utf8Size )
    {
        size_t unicodeSize = (_unicodeSize == MENGINE_UNKNOWN_SIZE) ? MENGINE_WCSLEN( _unicode ) + 1 : _unicodeSize + 1;

        char * sdl_utf8 = SDL_iconv_string( "UTF-8", SDL_UCS_wchar_t
            , (const char *)_unicode
            , unicodeSize * sizeof( WChar )
        );

        if( sdl_utf8 == nullptr )
        {
            return false;
        }

        size_t utf8Size = MENGINE_STRLEN( sdl_utf8 );

        if( _utf8 != nullptr )
        {
            if( _utf8Capacity < utf8Size )
            {
                SDL_free( sdl_utf8 );

                if( _utf8Size != nullptr )
                {
                    *_utf8Size = utf8Size;
                }

                return false;
            }

            Algorithm::copy( sdl_utf8, sdl_utf8 + utf8Size, _utf8 );

            _utf8[utf8Size] = '\0';
        }

        if( _utf8Size != nullptr )
        {
            *_utf8Size = utf8Size;
        }

        SDL_free( sdl_utf8 );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLUnicodeSystem::utf8ToUnicode( const Char * _utf8, size_t _utf8Size, WChar * const _unicode, size_t _unicodeCapacity, size_t * const _sizeUnicode )
    {
        size_t utf8Size = (_utf8Size == MENGINE_UNKNOWN_SIZE) ? MENGINE_STRLEN( _utf8 ) + 1 : _utf8Size + 1;

        char * sdl_unicode = SDL_iconv_string( SDL_UCS_wchar_t, "UTF-8"
            , _utf8
            , utf8Size * sizeof( Char )
        );

        if( sdl_unicode == nullptr )
        {
            return false;
        }

        WChar * sdl_unicode_wchar = (WChar *)sdl_unicode;

        size_t unicodeSize = MENGINE_WCSLEN( sdl_unicode_wchar );

        if( _unicode != nullptr )
        {
            if( _unicodeCapacity < unicodeSize )
            {
                SDL_free( sdl_unicode );

                if( _sizeUnicode != nullptr )
                {
                    *_sizeUnicode = unicodeSize;
                }

                return false;
            }

            Algorithm::copy( sdl_unicode_wchar, sdl_unicode_wchar + unicodeSize, _unicode );

            _unicode[unicodeSize] = L'\0';
        }

        if( _sizeUnicode != nullptr )
        {
            *_sizeUnicode = unicodeSize;
        }

        SDL_free( sdl_unicode );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
