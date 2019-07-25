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

    public:
        static const FilePath & none();
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<FilePath> VectorFilePath;
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct Hashgen<FilePath>
    {
        HashType operator()( const FilePath & _value ) const
        {
            return _value.hash();
        }
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        FilePath stringizeFilePath( const Char * _value );
        FilePath stringizeFilePathSize( const Char * _value, FilePath::size_type _size );
        FilePath stringizeFilePath( const String & _path );
        FilePath stringizeFilePath( const PathString & _path );
        FilePath stringizeFilePathLocal( const Char * _value, FilePath::size_type _size );
        FilePath stringizeFilePathLocal( const PathString & _path );
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Literals
    {
        //////////////////////////////////////////////////////////////////////////
        FilePath operator "" _fp( const Char * _value, size_t _size );
    }
    //////////////////////////////////////////////////////////////////////////
    using namespace Literals;
}
//////////////////////////////////////////////////////////////////////////
#define STRINGIZE_FILEPATH_LOCAL( str )\
	Helper::stringizeFilePathSize( str, (sizeof(str) - 1) )
//////////////////////////////////////////////////////////////////////////