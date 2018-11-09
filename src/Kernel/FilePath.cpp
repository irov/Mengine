#include "FilePath.h"

#include "Interface/StringizeServiceInterface.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        FilePath stringizeFilePath( const Char * _value, size_t _size )
        {
            ConstString cstr = Helper::stringizeStringSize( _value, (ConstStringHolder::size_type)_size );

            return FilePath( cstr );
        }
        //////////////////////////////////////////////////////////////////////////
        FilePath stringizeFilePath( const String & _path )
        {
            FilePath fp = Helper::stringizeFilePath( _path.c_str(), _path.size() );

            return fp;
        }
        //////////////////////////////////////////////////////////////////////////
        FilePath stringizeFilePath( const PathString & _path )
        {
            FilePath fp = Helper::stringizeFilePath( _path.c_str(), _path.size() );

            return fp;
        }
    }
}