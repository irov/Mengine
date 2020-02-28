#pragma once

#include "Kernel/FilePath.h"
#include "Kernel/ConstString.h"
#include "Kernel/PathString.h"
#include "Kernel/HashHelper.h"

#include "Config/Typedef.h"
#include "Config/String.h"
#include "Config/Vector.h"

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
        FilePath stringizeFileHashUnique( const Char * _value, FilePath::size_type _size, FilePath::hash_type _hash );
        //////////////////////////////////////////////////////////////////////////
        ConstString getFilePathExt( const FilePath & _filePath );
        //////////////////////////////////////////////////////////////////////////
        template<FilePath::hash_type Hash>
        FilePath stringizeFileTemplate( const Char * _value, FilePath::size_type _size )
        {
            static FilePath fp = Helper::stringizeFileHashUnique( _value, _size, Hash );

            return fp;
        }
    }
}
//////////////////////////////////////////////////////////////////////////
#define STRINGIZE_FILEPATH_LOCAL( STRING )\
    Helper::stringizeFileTemplate<STRING##_hash>( STRING, (sizeof(STRING) - 1) )
//////////////////////////////////////////////////////////////////////////
#define STRINGIZE_FILEPATH_LOCAL_I( STRING )\
    STRINGIZE_FILEPATH_LOCAL(#STRING)
//////////////////////////////////////////////////////////////////////////