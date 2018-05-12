#pragma once

#include "Factory/Factorable.h"

#include "stdex/intrusive_linked.h"

#include <stdint.h>

namespace Mengine
{
    class ConstStringHolder
        : public stdex::intrusive_linked<ConstStringHolder>
    {
    protected:
        ConstStringHolder();
        ConstStringHolder( const ConstStringHolder & _holder );
        ~ConstStringHolder();

    public:
        typedef size_t size_type;
        typedef int64_t hash_type;
        typedef char value_type;

    public:
        inline const char * c_str() const
        {
            return m_data;
        }

        inline size_type size() const
        {
            return m_size;
        }

        inline bool empty() const
        {
            return m_size == 0;
        }

    public:
        inline hash_type hash() const
        {
            return m_hash;
        }

    public:
        void setup( const char * _data, size_type _size, hash_type _hash );

    protected:
        const char * m_data;
        size_type m_size;

        hash_type m_hash;
    };
}