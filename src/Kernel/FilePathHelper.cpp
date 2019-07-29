#include "FilePathHelper.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/Assertion.h"

#include <stdarg.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        FilePath stringizeFilePath( const Char * _value )
        {
            ConstString cstr = Helper::stringizeString( _value );

            return FilePath( cstr );
        }
        //////////////////////////////////////////////////////////////////////////
        FilePath stringizeFilePathSize( const Char * _value, FilePath::size_type _size )
        {
            ConstString cstr = Helper::stringizeStringSize( _value, _size );

            return FilePath( cstr );
        }
        //////////////////////////////////////////////////////////////////////////
        FilePath stringizeFilePath( const String & _path )
        {
            const Char * value_str = _path.c_str();
            String::size_type value_size = _path.size();

            FilePath fp = Helper::stringizeFilePathSize( value_str, (FilePath::size_type)value_size );

            return fp;
        }
        //////////////////////////////////////////////////////////////////////////
        FilePath stringizeFilePath( const PathString & _path )
        {
            const Char * value_str = _path.c_str();
            PathString::size_type value_size = _path.size();

            FilePath fp = Helper::stringizeFilePathSize( value_str, value_size );

            return fp;
        }
        //////////////////////////////////////////////////////////////////////////
        FilePath stringizeFilePathLocal( const Char * _value, FilePath::size_type _size )
        {
            ConstString cstr = Helper::stringizeStringLocal( _value, _size );

            return FilePath( cstr );
        }
        //////////////////////////////////////////////////////////////////////////
        FilePath stringizeFilePathLocal( const PathString & _path )
        {
            const Char * value_str = _path.c_str();
            PathString::size_type value_size = _path.size();

            FilePath fp = Helper::stringizeFilePathLocal( value_str, value_size );

            return fp;
        }
        //////////////////////////////////////////////////////////////////////////
        FilePath stringizeFilePathFormat( const Char * _format, ... )
        {
            va_list args;
            va_start( args, _format );

            Char str[MENGINE_MAX_PATH] = { 0 };
            int size = ::vsnprintf( str, MENGINE_MAX_PATH - 1, _format, args );

            MENGINE_ASSERTION_FATAL( size >= 0, "invalid stringize file format '%s'"
                , _format
            );

            va_end( args );

            if( size == 0 )
            {
                return FilePath::none();
            }

            FilePath filePath = Helper::stringizeFilePathSize( str, size );

            return filePath;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Literals
    {
        //////////////////////////////////////////////////////////////////////////
        FilePath operator "" _fp( const Char * _value, size_t _size )
        {
            FilePath cstr = Helper::stringizeFilePathSize( _value, (FilePath::size_type)_size );

            return cstr;
        }
    }
}