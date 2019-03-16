#include "SDLUnicodeSystem.h"

#include "SDL_stdinc.h"

#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( UnicodeSystem, Mengine::SDLUnicodeSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static const Char * SDL_UCS_types[] = {"UCS-2-INTERNAL", "UCS-4-INTERNAL"};
    static const Char * SDL_UCS_wchar_t = SDL_UCS_types[sizeof( WChar ) / 2 - 1];
    //////////////////////////////////////////////////////////////////////////
    SDLUnicodeSystem::SDLUnicodeSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLUnicodeSystem::~SDLUnicodeSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLUnicodeSystem::unicodeToUtf8( const WChar * _unicode, size_t _unicodeSize, Char * _utf8, size_t _utf8Capacity, size_t * _utf8Size )
    {
        size_t unicodeSize = (_unicodeSize == MENGINE_UNICODE_UNKNOWN_SIZE) ? wcslen( _unicode ) + 1 : _unicodeSize + 1;

        char * sdl_utf8 = SDL_iconv_string( "UTF-8", SDL_UCS_wchar_t
            , (const char*)_unicode
            , unicodeSize * sizeof( WChar )
        );

        if( sdl_utf8 == nullptr )
        {
            return false;
        }

        size_t utf8Size = strlen( sdl_utf8 );

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

            std::copy( sdl_utf8, sdl_utf8 + utf8Size, _utf8 );
        }

        if( _utf8 != nullptr )
        {
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
    bool SDLUnicodeSystem::utf8ToUnicode( const Char * _utf8, size_t _utf8Size, WChar * _unicode, size_t _unicodeCapacity, size_t * _sizeUnicode )
    {
        size_t utf8Size = (_utf8Size == MENGINE_UNICODE_UNKNOWN_SIZE) ? strlen( _utf8 ) + 1 : _utf8Size + 1;

        wchar_t * sdl_unicode = (wchar_t*)SDL_iconv_string( SDL_UCS_wchar_t, "UTF-8"
            , _utf8
            , utf8Size * sizeof( Char )
        );

        if( sdl_unicode == nullptr )
        {
            return false;
        }

        size_t unicodeSize = wcslen( sdl_unicode );

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

            std::copy( sdl_unicode, sdl_unicode + unicodeSize, _unicode );
        }

        if( _unicode != nullptr )
        {
            _unicode[unicodeSize] = L'\0';
        }

        if( _sizeUnicode != nullptr )
        {
            *_sizeUnicode = unicodeSize;
        }

        SDL_free( sdl_unicode );

        return true;
    }
}
