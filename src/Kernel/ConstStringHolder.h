#pragma once

#include "Kernel/IntrusiveLinked.h"

#include "Config/Typedef.h"
#include "Config/Char.h"
#include "Config/HashType.h"

namespace Mengine
{
    class ConstStringHolder
        : public IntrusiveLinked<ConstStringHolder>
    {
    protected:
        ConstStringHolder();
        virtual ~ConstStringHolder();

    public:
        ConstStringHolder( const ConstStringHolder & _holder );

    protected:
        ConstStringHolder & operator = ( const ConstStringHolder & _holder );

    public:
        typedef size_t size_type;
        typedef HashType hash_type;
        typedef Char value_type;

    public:
        MENGINE_CONSTEXPR const value_type * c_str() const
        {
            return m_data;
        }

        MENGINE_CONSTEXPR size_type size() const
        {
            return m_size;
        }

        MENGINE_CONSTEXPR bool empty() const
        {
            return m_size == 0;
        }

    public:
        MENGINE_CONSTEXPR hash_type hash() const
        {
            return m_hash;
        }

    protected:
        MENGINE_CONSTEXPR const value_type * data() const
        {
            return m_data;
        }

    public:
        void setup( const value_type * _data, size_type _size, hash_type _hash );

    protected:
        const value_type * m_data;
        size_type m_size;

        hash_type m_hash;
    };
}