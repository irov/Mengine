#pragma once

#include "Config/Typedef.h"
#include "Config/Char.h"

#include "Kernel/Factorable.h"

#include "stdex/intrusive_linked.h"

namespace Mengine
{
    class ConstStringHolder
        : public stdex::intrusive_linked<ConstStringHolder>
    {
    protected:
        ConstStringHolder();
        ConstStringHolder( const ConstStringHolder & _holder );
        virtual ~ConstStringHolder();

    public:
        typedef uint32_t size_type;
        typedef int64_t hash_type;
        typedef Char value_type;

    public:
        inline const value_type * c_str() const noexcept
        {
            return m_data;
        }

        inline size_type size() const noexcept
        {
            return m_size;
        }

        inline bool empty() const noexcept
        {
            return m_size == 0;
        }

    public:
        inline hash_type hash() const noexcept
        {
            return m_hash;
        }

    public:
        void setup( const value_type * _data, size_type _size, hash_type _hash );

    protected:
        const value_type * m_data;
        size_type m_size;

        hash_type m_hash;
    };
}