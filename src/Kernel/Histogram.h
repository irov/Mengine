#pragma once

#include "Config/Algorithm.h"

namespace Mengine
{
    template<class T, size_t Count>
    class Histogram
    {
    public:
        Histogram()
            : m_size( 0 )
        {
            Algorithm::fill_n( m_values, Count, T() );
        }

        ~Histogram()
        {
        }

    public:
        void add( T _value )
        {
            if( m_size < Count )
            {
                m_values[m_size++] = _value;
            }
            else
            {
                for( size_t i = 1; i != Count; ++i )
                {
                    m_values[i - 1] = m_values[i];
                }

                m_values[Count - 1] = _value;
            }
        }

    public:
        const T * getValues() const
        {
            return m_values;
        }

        T getLastValue() const
        {
            if( m_size == 0 )
            {
                return T();
            }

            T value = m_values[m_size - 1];

            return value;
        }

        T getMinValue() const
        {
            const T * value = Algorithm::min_element( m_values, m_values + Count );

            return *value;
        }

        T getMaxValue() const
        {
            const T * value = Algorithm::max_element( m_values, m_values + Count );

            return *value;
        }

    protected:
        T m_values[Count];
        size_t m_size;
    };
}