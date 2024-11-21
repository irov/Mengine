#include "FilePathHelper.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/Assertion.h"

#include "Config/StdIO.h"
#include "Config/StdArg.h"
#include "Config/StdString.h"
#include "Config/Path.h"

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
        FilePath stringizeFilePathLocal( const Char * _value, FilePath::size_type _size )
        {
            ConstString cstr = Helper::stringizeStringLocal( _value, _size );

            return FilePath( cstr );
        }
        //////////////////////////////////////////////////////////////////////////
        FilePath stringizeFilePathFormat( const Char * _format, ... )
        {
            Path str = {'\0'};

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _format );

            int32_t size_vsnprintf = MENGINE_VSNPRINTF( str, MENGINE_MAX_PATH, _format, args );

            MENGINE_VA_LIST_END( args );

            MENGINE_ASSERTION_FATAL( size_vsnprintf >= 0, "invalid stringize file format '%s'"
                , _format
            );

            if( size_vsnprintf == 0 )
            {
                return FilePath::none();
            }

            FilePath filePath = Helper::stringizeFilePathSize( str, size_vsnprintf );

            return filePath;
        }
        //////////////////////////////////////////////////////////////////////////
        FilePath stringizeFileHashUnique( const Char * _value, FilePath::size_type _size, FilePath::hash_type _hash )
        {
            ConstString constString = Helper::stringizeStringHashUnique( _value, _size, _hash );

            return FilePath( constString );
        }
        //////////////////////////////////////////////////////////////////////////
        ConstString getFilePathExt( const FilePath & _filePath )
        {
            const Char * str_filePath = _filePath.c_str();

            const Char * dot_find = StdString::strrchr( str_filePath, '.' );

            if( dot_find == nullptr )
            {
                return ConstString::none();
            }

            ConstString ext = Helper::stringizeString( dot_find + 1 );

            return ext;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}