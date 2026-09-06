#pragma once

#include "Kernel/InitializerList.h"
#include "Kernel/StringCopy.h"
#include "Kernel/StringLength.h"

#include "Config/StdAssert.h"
#include "Config/StdDef.h"

namespace Mengine
{
    template<class T, size_t Count, size_t Size>
    class ArrayTStrings
    {
    public:
        typedef InitializerList<const T *> InitializerListValues;

    public:
        ArrayTStrings()
        {
        }

        ArrayTStrings( InitializerListValues _values )
        {
            this->append( _values );
        }

        ~ArrayTStrings()
        {
        }

    public:
        void append( const T * _value )
        {
            assert( _value != nullptr );
            assert( m_size < Count );
            assert( Helper::stringLength( _value ) < Size );

            T * value = m_values[m_size];
            Helper::stringCopy( value, _value );

            ++m_size;
        }

        void append( InitializerListValues _values )
        {
            for( const T * value : _values )
            {
                this->append( value );
            }
        }

    public:
        const T * operator [] ( size_t _index ) const
        {
            assert( _index < m_size );

            return m_values[_index];
        }

        size_t size() const
        {
            return m_size;
        }

        bool empty() const
        {
            return m_size == 0;
        }

        void clear()
        {
            m_size = 0;
        }

    protected:
        T m_values[Count][Size] = {};
        size_t m_size = 0;
    };
}
