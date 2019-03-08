#include "Win32UnicodeSystem.h"

#include "Interface/PlatformInterface.h"

#include "Kernel/Logger.h"

#include "Environment/Windows/WindowsIncluder.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( UnicodeSystem, Mengine::Win32UnicodeSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32UnicodeSystem::Win32UnicodeSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32UnicodeSystem::~Win32UnicodeSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32UnicodeSystem::unicodeToUtf8( const WChar * _unicode, size_t _unicodeSize, Char * _utf8, size_t _utf8Capacity, size_t * _utf8Size )
    {
#   if (WINVER >= 0x0600)
        DWORD dwConversionFlags = WC_ERR_INVALID_CHARS;
#   else
        DWORD dwConversionFlags = 0;
#   endif

        int32_t int_unicodeSize = static_cast<int32_t>(_unicodeSize);
        int32_t int_utf8Capacity = static_cast<int32_t>(_utf8Capacity);

        if( int_unicodeSize == MENGINE_UNICODE_UNKNOWN_SIZE && int_utf8Capacity != 0 )
        {
            ++int_utf8Capacity;
        }

        int32_t utf8_size = ::WideCharToMultiByte(
            CP_UTF8
            , dwConversionFlags
            , _unicode
            , int_unicodeSize
            , _utf8
            , int_utf8Capacity
            , NULL
            , NULL
        );

        if( utf8_size == 0 && _unicodeSize != 0 )
        {
            LOGGER_ERROR( "unicode '%ls' WideCharToMultiByte 0"
                , _unicode
            );

            return false;
        }

        if( int_unicodeSize == MENGINE_UNICODE_UNKNOWN_SIZE )
        {
            --utf8_size;
        }

        if( _utf8 != nullptr )
        {
            _utf8[utf8_size] = '\0';
        }

        if( _utf8Size != nullptr )
        {
            size_t u_utf8_size = static_cast<size_t>(utf8_size);
            *_utf8Size = u_utf8_size;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32UnicodeSystem::utf8ToUnicode( const Char * _utf8, size_t _utf8Size, WChar * _unicode, size_t _unicodeCapacity, size_t * _sizeUnicode )
    {
        int32_t int_utf8Size = static_cast<int32_t>(_utf8Size);
        int32_t int_unicodeCapacity = static_cast<int32_t>(_unicodeCapacity);

        if( int_utf8Size == MENGINE_UNICODE_UNKNOWN_SIZE && int_unicodeCapacity != 0 )
        {
            ++int_unicodeCapacity;
        }

        int32_t wc_size = ::MultiByteToWideChar(
            CP_UTF8
            , MB_ERR_INVALID_CHARS
            , _utf8
            , int_utf8Size
            , _unicode
            , int_unicodeCapacity
        );

        if( (wc_size == 0 || wc_size == 0xFFFD) && _utf8Size != 0 )
        {
            DWORD err = GetLastError();

            Char wstr_err[1024];
            PLATFORM_SERVICE()
                ->getErrorMessage( err, wstr_err );

            LOGGER_ERROR( "utf8 '%s' MultiByteToWideChar '%ls'"
                , _utf8
                , wstr_err
            );

            return false;
        }

        if( int_utf8Size == MENGINE_UNICODE_UNKNOWN_SIZE )
        {
            --wc_size;
        }

        if( _unicode != nullptr )
        {
            _unicode[wc_size] = L'\0';
        }

        if( _sizeUnicode != nullptr )
        {
            size_t u_wc_size = static_cast<size_t>(wc_size);
            *_sizeUnicode = u_wc_size;
        }

        return true;
    }
}