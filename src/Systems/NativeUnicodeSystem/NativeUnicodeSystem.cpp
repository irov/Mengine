#include "NativeUnicodeSystem.h"

#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Config/StdString.h"
#include "Config/Algorithm.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( UnicodeSystem, Mengine::NativeUnicodeSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    NativeUnicodeSystem::NativeUnicodeSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    NativeUnicodeSystem::~NativeUnicodeSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool NativeUnicodeSystem::_initializeService()
    {
        //Empty
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NativeUnicodeSystem::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool NativeUnicodeSystem::unicodeToUtf8( const WChar * _unicode, size_t _unicodeSize, Char * const _utf8, size_t _utf8Capacity, size_t * const _utf8Size )
    {
        size_t unicodeSize = (_unicodeSize == (size_t)-1) ? StdString::wcslen( _unicode ) : _unicodeSize;

        size_t utf8Size = 0;
        for( size_t i = 0; i != unicodeSize; ++i )
        {
            uint32_t wc = _unicode[i];

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
                    _utf8[utf8Size + 0] = (Char)(0xC0 | (wc >> 6));
                    _utf8[utf8Size + 1] = (Char)(0x80 | (wc & 0x3F));
                }

                utf8Size += 2;
            }
            else if( wc < 0x10000 )
            {
                if( _utf8 != nullptr && utf8Size + 2 < _utf8Capacity )
                {
                    _utf8[utf8Size + 0] = (Char)(0xE0 | (wc >> 12));
                    _utf8[utf8Size + 1] = (Char)(0x80 | ((wc >> 6) & 0x3F));
                    _utf8[utf8Size + 2] = (Char)(0x80 | (wc & 0x3F));
                }

                utf8Size += 3;
            }
            else if( wc < 0x110000 )
            {
                if( _utf8 != nullptr && utf8Size + 3 < _utf8Capacity )
                {
                    _utf8[utf8Size + 0] = (Char)(0xF0 | (wc >> 18));
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
    bool NativeUnicodeSystem::utf8ToUnicode( const Char * _utf8, size_t _utf8Size, WChar * const _unicode, size_t _unicodeCapacity, size_t * const _sizeUnicode )
    {
        size_t utf8Index = 0;
        size_t unicodeIndex = 0;
        size_t unicodeSize = 0;

        while( utf8Index < _utf8Size && _utf8[utf8Index] != '\0' )
        {
            WChar wc = 0;

            Char c = _utf8[utf8Index];

            if( (c & 0x80) == 0 )
            {
                wc = c;

                utf8Index += 1;
            }
            else if( (c & 0xE0) == 0xC0 )
            {
                if( utf8Index + 1 >= _utf8Size )
                {
                    return false;
                }

                wc = (c & 0x1F) << 6;
                wc |= (_utf8[utf8Index + 1] & 0x3F) << 0;

                utf8Index += 2;
            }
            else if( (c & 0xF0) == 0xE0 )
            {
                if( utf8Index + 2 >= _utf8Size )
                {
                    return false;
                }

                wc = (c & 0x0F) << 12;
                wc |= (_utf8[utf8Index + 1] & 0x3F) << 6;
                wc |= (_utf8[utf8Index + 2] & 0x3F) << 0;

                utf8Index += 3;
            }
            else if( (c & 0xF8) == 0xF0 )
            {
                if( utf8Index + 3 >= _utf8Size )
                {
                    return false;
                }

                wc = (c & 0x07) << 18;
                wc |= (_utf8[utf8Index + 1] & 0x3F) << 12;
                wc |= (_utf8[utf8Index + 2] & 0x3F) << 6;
                wc |= (_utf8[utf8Index + 3] & 0x3F) << 0;

                utf8Index += 4;
            }
            else
            {
                return false;
            }

            if( _unicode != nullptr )
            {
                if( unicodeIndex >= _unicodeCapacity )
                {
                    return false;
                }

                _unicode[unicodeIndex] = wc;
            }

            unicodeIndex += 1;
            unicodeSize += 1;
        }

        if( _unicode != nullptr )
        {
            if( unicodeIndex >= _unicodeCapacity )
            {
                return false;
            }

            _unicode[unicodeIndex] = L'\0';
        }

        if( _sizeUnicode != nullptr )
        {
            *_sizeUnicode = unicodeSize;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
