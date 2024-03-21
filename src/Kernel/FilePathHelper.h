#pragma once

#include "Kernel/FilePath.h"
#include "Kernel/ConstString.h"
#include "Kernel/HashHelper.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        FilePath stringizeFilePath( const Char * _value );
        FilePath stringizeFilePathSize( const Char * _value, FilePath::size_type _size );
        FilePath stringizeFilePathLocal( const Char * _value, FilePath::size_type _size );
        FilePath stringizeFilePathFormat( const Char * _format, ... ) MENGINE_ATTRIBUTE_FORMAT_STRING( 1, 2 );
        FilePath stringizeFileHashUnique( const Char * _value, FilePath::size_type _size, FilePath::hash_type _hash );
        //////////////////////////////////////////////////////////////////////////
        namespace Detail
        {
            //////////////////////////////////////////////////////////////////////////
            template<class T>
            struct FilePathHelper
            {
                FilePath operator() ( const T & _value ) const
                {
                    const FilePath::value_type * value_str = _value.c_str();
                    FilePath::size_type value_size = (FilePath::size_type)_value.size();

                    FilePath fp = Helper::stringizeFilePathSize( value_str, value_size );

                    return fp;
                }
            };
            //////////////////////////////////////////////////////////////////////////
            template<size_t N>
            struct FilePathHelper<Char[N]>
            {
                FilePath operator() ( const Char * _value ) const
                {
                    const FilePath::value_type * value_str = _value;

                    FilePath fp = Helper::stringizeFilePath( value_str );

                    return fp;
                }
            };
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        FilePath stringizeFilePath( const T & _value )
        {
            FilePath fp = Detail::FilePathHelper<T>()(_value);

            return fp;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        FilePath stringizeFilePathLocal( const T & _value )
        {
            const FilePath::value_type * value_str = _value.c_str();
            FilePath::size_type value_size = _value.size();

            FilePath fp = Helper::stringizeFilePathLocal( value_str, value_size );

            return fp;
        }
        //////////////////////////////////////////////////////////////////////////
        ConstString getFilePathExt( const FilePath & _filePath );
        //////////////////////////////////////////////////////////////////////////
        template<FilePath::hash_type Hash>
        const FilePath & stringizeFileTemplate( const Char * _value, FilePath::size_type _size )
        {
            static FilePath fp = Helper::stringizeFileHashUnique( _value, _size, Hash );

            return fp;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
//////////////////////////////////////////////////////////////////////////
#define STRINGIZE_FILEPATH_LOCAL(STRING)\
    Helper::stringizeFileTemplate<STRING##_hash>( STRING, (sizeof(STRING) - 1) )
//////////////////////////////////////////////////////////////////////////