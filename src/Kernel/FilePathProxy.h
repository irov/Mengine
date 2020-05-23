#pragma once

#include "Kernel/FilePathHelper.h"

namespace Mengine
{
    template<FilePath::hash_type Hash>
    class FilePathProxy
    {
    public:
        FilePathProxy( const Char * _string, FilePath::size_type _size )
            : m_string( _string )
            , m_size( _size )
        {
        }

    public:
        const FilePath & c_filepath() const
        {
            static FilePath cstr = Helper::stringizeFileHashUnique( m_string, m_size, Hash );

            return cstr;
        }

        operator const FilePath & () const
        {
            return this->c_filepath();
        }

    protected:
        const Char * m_string;
        FilePath::size_type m_size;
    };
}