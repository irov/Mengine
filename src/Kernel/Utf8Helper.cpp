#include "Utf8Helper.h"

#include "utf8.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool Utf8NextCode( const Utf8 ** _utf8, const Utf8 * _utf8End, uint32_t * const _utf8Code )
        {
            uint32_t code;
            utf8::internal::utf_error err = utf8::internal::validate_next( *_utf8, _utf8End, code );

            if( err != utf8::internal::UTF8_OK )
            {
                return false;
            }

            *_utf8Code = code;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool Utf8Validate( const Utf8 * _utf8, const Utf8 * _utf8End, const Utf8 ** const _utf8Invalid )
        {
            const Utf8 * utf8Invalid = utf8::find_invalid( _utf8, _utf8End );

            if( utf8Invalid != _utf8End )
            {
                if( _utf8Invalid != nullptr )
                { 
                    *_utf8Invalid = utf8Invalid;
                }

                return false;
            }

            return true;            
        }
        //////////////////////////////////////////////////////////////////////////
        const Utf8 * Utf8ReplaceInvalid( const Utf8 * _utf8, const Utf8 * _utf8End, Utf8 * const _utf8Out )
        {
            const Utf8 * utf8End = utf8::replace_invalid( _utf8, _utf8End, _utf8Out );

            return utf8End;
        }
        //////////////////////////////////////////////////////////////////////////
        bool Utf32ToUtf8( Utf32 _utf32, Utf8 _utf8 [5] )
        {
            if( _utf32 <= 0x7F )
            {
                _utf8[0] = (Utf8)_utf32;
                _utf8[1] = '\0';
            }
            else if( _utf32 <= 0x7FF )
            {
                _utf8[0] = 0xC0 | (Utf8)((_utf32 >> 6) & 0x1F);
                _utf8[1] = 0x80 | (Utf8)(_utf32 & 0x3F);
                _utf8[2] = '\0';
            }
            else if( _utf32 <= 0xFFFF )
            {
                _utf8[0] = 0xE0 | (Utf8)((_utf32 >> 12) & 0x0F);
                _utf8[1] = 0x80 | (Utf8)((_utf32 >> 6) & 0x3F);
                _utf8[2] = 0x80 | (Utf8)(_utf32 & 0x3F);
                _utf8[3] = '\0';
            }
            else if( _utf32 <= 0x10FFFF )
            {
                _utf8[0] = 0xF0 | (Utf8)((_utf32 >> 18) & 0x0F);
                _utf8[1] = 0x80 | (Utf8)((_utf32 >> 12) & 0x3F);
                _utf8[2] = 0x80 | (Utf8)((_utf32 >> 6) & 0x3F);
                _utf8[3] = 0x80 | (Utf8)(_utf32 & 0x3F);
                _utf8[4] = '\0';
            }
            else
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}