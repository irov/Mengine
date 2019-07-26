#include "FilePathHelper.h"

#include "Kernel/ConstStringHelper.h"

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