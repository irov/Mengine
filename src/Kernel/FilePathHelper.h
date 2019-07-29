#pragma once

#include "Config/Typedef.h"
#include "Config/String.h"
#include "Config/Vector.h"

#include "Kernel/FilePath.h"
#include "Kernel/PathString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        FilePath stringizeFilePath( const Char * _value );
        FilePath stringizeFilePath( const String & _path );
        FilePath stringizeFilePath( const PathString & _path );
        FilePath stringizeFilePathSize( const Char * _value, FilePath::size_type _size );
        FilePath stringizeFilePathLocal( const Char * _value, FilePath::size_type _size );
        FilePath stringizeFilePathLocal( const PathString & _path );
        FilePath stringizeFilePathFormat( const Char * _format, ... );
    }
}
//////////////////////////////////////////////////////////////////////////
#define STRINGIZE_FILEPATH_LOCAL( str )\
	Helper::stringizeFilePathSize( str, (sizeof(str) - 1) )
//////////////////////////////////////////////////////////////////////////