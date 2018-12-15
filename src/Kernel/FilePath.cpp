#include "FilePath.h"

#include "Interface/StringizeServiceInterface.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        FilePath stringizeFilePath( const Char * _value )
        {
            ConstString cstr = Helper::stringizeString( _value );

            return FilePath( cstr );
        }
        //////////////////////////////////////////////////////////////////////////
        FilePath stringizeFilePathSize( const Char * _value, size_t _size )
        {
            ConstString cstr = Helper::stringizeStringSize( _value, _size );

            return FilePath( cstr );
        }
        //////////////////////////////////////////////////////////////////////////
        FilePath stringizeFilePath( const String & _path )
        {
            FilePath fp = Helper::stringizeFilePathSize( _path.c_str(), _path.size() );

            return fp;
        }
        //////////////////////////////////////////////////////////////////////////
        FilePath stringizeFilePath( const PathString & _path )
        {
            FilePath fp = Helper::stringizeFilePathSize( _path.c_str(), _path.size() );

            return fp;
        }
    }
}