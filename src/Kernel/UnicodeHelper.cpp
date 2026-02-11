#include "UnicodeHelper.h"

#include "Config/StdString.h"

#include "utf8/utf8.hpp"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool utf8NextCode( const Utf8 ** _utf8, const Utf8 * _utf8End, uint32_t * const _utf8Code )
        {
            const Char * nextUtf8 = utf8_next_code( *_utf8, _utf8End, _utf8Code );

            if( nextUtf8 == NULL )
            {
                return false;
            }

            *_utf8 = nextUtf8;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool utf8Validate( const Utf8 * _utf8, const Utf8 * _utf8End, const Utf8 ** const _utf8Invalid )
        {
            const Utf8 * utf8Invalid = utf8_validate( _utf8, _utf8End );

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
        const Utf8 * utf8ReplaceInvalid( const Utf8 * _utf8, const Utf8 * _utf8End, Utf8 * const _utf8Out )
        {
            const Utf8 * utf8End = utf8_replace_invalid( _utf8, _utf8End, _utf8Out );

            return utf8End;
        }
        //////////////////////////////////////////////////////////////////////////
        bool utf32ToUtf8( Utf32 _utf32, Utf8 _utf8[5] )
        {
            size_t size = utf8_from_unicode32_symbol( _utf32, _utf8 );

            if( size == UTF8_UNKNOWN )
            {
                return false;
            }

            _utf8[size] = '\0';

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool unicodeToUtf8( const WChar * _unicode, String * const _utf8 )
        {
            size_t utf8Size = utf8_from_unicode_size( _unicode );

            if( utf8Size == UTF8_UNKNOWN )
            {
                _utf8->clear();

                return true;
            }

            _utf8->resize( utf8Size );

            Char * utf8_data = _utf8->data();

            utf8_from_unicode( _unicode, utf8_data, utf8Size );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool unicodeToUtf8( const WChar * _unicode, size_t _unicodeSize, String * const _utf8 )
        {
            size_t utf8Size = utf8_from_unicodez_size( _unicode, _unicodeSize );

            if( utf8Size == UTF8_UNKNOWN )
            {
                _utf8->clear();

                return true;
            }

            _utf8->resize( utf8Size );

            Char * utf8_data = _utf8->data();

            utf8_from_unicodez( _unicode, _unicodeSize, utf8_data, utf8Size );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool unicodeToUtf8( const WString & _unicode, String * const _utf8 )
        {
            const WChar * unicodeStr = _unicode.c_str();
            size_t unicodeSize = _unicode.size();

            bool result = Helper::unicodeToUtf8( unicodeStr, unicodeSize, _utf8 );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        bool unicodeToUtf8( const WChar * _unicode, Char * const _utf8, size_t _utf8Capacity, size_t * const _utf8Size )
        {
            size_t utf8Size = utf8_from_unicode_size( _unicode );

            if( utf8Size == UTF8_UNKNOWN )
            {
                return false;
            }

            if( utf8Size + 1 >= _utf8Capacity )
            {
                return false;
            }

            utf8_from_unicode( _unicode, _utf8, _utf8Capacity );

            _utf8[utf8Size] = '\0';

            if( _utf8Size != nullptr )
            {
                *_utf8Size = utf8Size;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool unicodeToUtf8( const WChar * _unicode, size_t _unicodeSize, Char * const _utf8, size_t _utf8Capacity, size_t * const _utf8Size )
        {
            size_t utf8Size = utf8_from_unicodez_size( _unicode, _unicodeSize );

            if( utf8Size == UTF8_UNKNOWN )
            {
                return false;
            }

            if( utf8Size + 1 >= _utf8Capacity )
            {
                return false;
            }

            utf8_from_unicodez( _unicode, _unicodeSize, _utf8, _utf8Capacity );

            _utf8[utf8Size] = '\0';

            if( _utf8Size != nullptr )
            {
                *_utf8Size = utf8Size;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool unicodeToUtf8( const WString & _unicode, Char * const _utf8, size_t _utf8Capacity, size_t * const _utf8Size )
        {
            const WChar * unicodeStr = _unicode.c_str();
            size_t unicodeSize = _unicode.size();

            bool successful = Helper::unicodeToUtf8( unicodeStr, unicodeSize, _utf8, _utf8Capacity, _utf8Size );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        bool utf8ToUnicode( const Char * _utf8, WChar * const _unicode, size_t _unicodeCapacity, size_t * const _unicodeSize )
        {
            size_t unicodeSize = utf8_to_unicode( _utf8, _unicode, _unicodeCapacity );

            if( unicodeSize == UTF8_UNKNOWN )
            {
                return false;
            }

            if( unicodeSize + 1 >= _unicodeCapacity )
            {
                return false;
            }

            _unicode[unicodeSize] = L'\0';

            if( _unicodeSize != nullptr )
            {
                *_unicodeSize = unicodeSize;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool utf8ToUnicode( const Char * _utf8, WString * const _unicode )
        {
            size_t unicodeSize = utf8_to_unicode_size( _utf8 );

            if( unicodeSize == UTF8_UNKNOWN )
            {
                return false;
            }

            if( unicodeSize == 0 )
            {
                _unicode->clear();

                return true;
            }

            _unicode->resize( unicodeSize );

            WChar * unicode_data = _unicode->data();

            utf8_to_unicode( _utf8, unicode_data, unicodeSize );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool utf8ToUnicode( const Char * _utf8, size_t _utf8Size, WChar * const _unicode, size_t _unicodeCapacity, size_t * const _unicodeSize )
        {
            size_t unicodeSize = utf8_to_unicodez( _utf8, _utf8Size, _unicode, _unicodeCapacity );

            if( unicodeSize == UTF8_UNKNOWN )
            {
                return false;
            }

            if( unicodeSize + 1 >= _unicodeCapacity )
            {
                return false;
            }

            _unicode[unicodeSize] = L'\0';

            if( _unicodeSize != nullptr )
            {
                *_unicodeSize = unicodeSize;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool utf8ToUnicode( const Char * _utf8, size_t _utf8Size, WString * const _unicode )
        {
            size_t unicodeSize = utf8_to_unicodez_size( _utf8, _utf8Size );

            if( unicodeSize == UTF8_UNKNOWN )
            {
                return false;
            }

            if( unicodeSize == 0 )
            {
                _unicode->clear();

                return true;
            }

            _unicode->resize( unicodeSize );

            WChar * unicode_data = _unicode->data();

            utf8_to_unicodez( _utf8, _utf8Size, unicode_data, unicodeSize );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool utf8ToUnicode( const String & _utf8, WString * const _unicode )
        {
            const Char * utf8Str = _utf8.c_str();
            size_t utf8Size = _utf8.size();

            bool result = Helper::utf8ToUnicode( utf8Str, utf8Size, _unicode );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        bool utf8ToUnicode( const String & _utf8, WChar * const _unicode, size_t _unicodeCapacity, size_t * const _unicodeSize )
        {
            const Char * utf8Str = _utf8.c_str();
            size_t utf8Size = _utf8.size();

            bool result = Helper::utf8ToUnicode( utf8Str, utf8Size, _unicode, _unicodeCapacity, _unicodeSize );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        bool utf8ToUnicode( const ConstString & _utf8, WString * const _unicode )
        {
            const Char * utf8Str = _utf8.c_str();
            size_t utf8Size = _utf8.size();

            bool result = Helper::utf8ToUnicode( utf8Str, utf8Size, _unicode );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        bool utf8ToUnicode( const ConstString & _utf8, WChar * const _unicode, size_t _unicodeCapacity, size_t * const _unicodeSize )
        {
            const Char * utf8Str = _utf8.c_str();
            size_t utf8Size = _utf8.size();

            bool result = Helper::utf8ToUnicode( utf8Str, utf8Size, _unicode, _unicodeCapacity, _unicodeSize );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        bool utf8ToUnicode( const PathString & _utf8, WString * const _unicode )
        {
            const Char * utf8Str = _utf8.c_str();
            size_t utf8Size = _utf8.size();

            bool result = Helper::utf8ToUnicode( utf8Str, utf8Size, _unicode );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        bool utf8ToUnicode( const PathString & _utf8, WChar * const _unicode, size_t _unicodeCapacity, size_t * const _unicodeSize )
        {
            const Char * utf8Str = _utf8.c_str();
            size_t utf8Size = _utf8.size();

            bool result = Helper::utf8ToUnicode( utf8Str, utf8Size, _unicode, _unicodeCapacity, _unicodeSize );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        bool utf8ToUnicode( const FilePath & _utf8, WString * const _unicode )
        {
            const Char * utf8Str = _utf8.c_str();
            size_t utf8Size = _utf8.size();

            bool result = Helper::utf8ToUnicode( utf8Str, utf8Size, _unicode );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        bool utf8ToUnicode( const FilePath & _utf8, WChar * const _unicode, size_t _unicodeCapacity, size_t * const _unicodeSize )
        {
            const Char * utf8Str = _utf8.c_str();
            size_t utf8Size = _utf8.size();

            bool result = Helper::utf8ToUnicode( utf8Str, utf8Size, _unicode, _unicodeCapacity, _unicodeSize );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}