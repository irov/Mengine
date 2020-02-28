#pragma once

#include "Config/Typedef.h"
#include "Config/String.h"
#include "Config/Vector.h"

#include "Kernel/FilePath.h"
#include "Kernel/ConstString.h"
#include "Kernel/PathString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        FilePath stringizeFilePath( const Char * _value );
        FilePath stringizeFilePath( const String & _filePath );
        FilePath stringizeFilePath( const PathString & _filePath );
        FilePath stringizeFilePathSize( const Char * _value, FilePath::size_type _size );
        FilePath stringizeFilePathLocal( const Char * _value, FilePath::size_type _size );
        FilePath stringizeFilePathLocal( const PathString & _filePath );
        FilePath stringizeFilePathFormat( const Char * _format, ... );
        //////////////////////////////////////////////////////////////////////////
        ConstString getFilePathExt( const FilePath & _filePath );
        //////////////////////////////////////////////////////////////////////////
        template<FilePath::hash_type Hash>
        FilePath stringizeFileTemplate( const Char * _value, FilePath::size_type _size )
        {
            static ConstString cstr = Helper::stringizeStringHashUnique( _value, _size, Hash );

            return FilePath( cstr );
        }
    }
}
//////////////////////////////////////////////////////////////////////////
#define STRINGIZE_FILEPATH_LOCAL( STRING )\
    Helper::stringizeFileTemplate<STRING##_hash>( STRING, (sizeof(STRING) - 1) )
//////////////////////////////////////////////////////////////////////////