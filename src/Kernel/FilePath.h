#pragma once

#include "Config/Typedef.h"
#include "Config/String.h"
#include "Config/Vector.h"

#include "Interface/ServiceInterface.h"
#include "Interface/StringizeInterface.h"

#include "Kernel/ConstString.h"

#include "stdex/array_string.h"

namespace Mengine
{
#ifndef MENGINE_MAX_PATH
#   define MENGINE_MAX_PATH 260
#endif

#define MENGINE_FILE_PATH_INVALID_ID	((uint32_t)-1)

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

    //typedef ConstString FilePath; //UTF8
    typedef stdex::array_string<MENGINE_MAX_PATH> PathString;

    typedef Vector<FilePath> VectorFilePath;

    namespace Helper
    {
        FilePath emptyPath();

        FilePath concatenationFilePath( const FilePath & _left, const FilePath & _right );

        bool makeFullPath( const ConstString & _fileGroupName, const FilePath & _fileName, FilePath & _fullPath );
        FilePath getPathFolder( const FilePath & _fullpath );

        //////////////////////////////////////////////////////////////////////////
        FilePath stringizeFilePath( const Char * _value, size_t _size );
        FilePath stringizeFilePath( const String & _path );
        FilePath stringizeFilePath( const PathString & _path );
        //////////////////////////////////////////////////////////////////////////
        FilePath unicodeToFilePath( const WString & _unicode );
        //////////////////////////////////////////////////////////////////////////
        void pathCorrectBackslash( WChar * _out, const WChar * _in );
        void pathRemoveBackslash( WChar * _path );
        bool pathRemoveFileSpec( WChar * _in );
        //////////////////////////////////////////////////////////////////////////
    }

#	define STRINGIZE_FILEPATH_LOCAL( str )\
	Helper::stringizeFilePath( str, (sizeof(str) - 1) )
}