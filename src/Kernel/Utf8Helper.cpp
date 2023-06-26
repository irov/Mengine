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
    }
}