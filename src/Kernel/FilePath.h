#pragma once

#include "Config/Typedef.h"
#include "Config/String.h"
#include "Config/Vector.h"

#include "Kernel/ConstString.h"
#include "Kernel/PathString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FilePath
        : public ConstString
    {
    public:
        FilePath() noexcept
        {
        }

        explicit FilePath( const ConstString & _cst )
            : ConstString( _cst )
        {
        }
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<FilePath> VectorFilePath;
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        FilePath stringizeFilePath( const Char * _value, size_t _size );
        FilePath stringizeFilePath( const String & _path );
        FilePath stringizeFilePath( const PathString & _path );
    }
    //////////////////////////////////////////////////////////////////////////
#	define STRINGIZE_FILEPATH_LOCAL( str )\
	Helper::stringizeFilePath( str, (sizeof(str) - 1) )
}