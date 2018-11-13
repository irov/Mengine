#pragma once

#include "Assertion.h"

namespace Mengine
{
    template<class T>
    class AssertionMemoryPanicOperator
    {
    public:
        AssertionMemoryPanicOperator( const T & _value )
            : m_value( _value )
        {
        }

    public:
        const T & operator << ( const AssertionOperator & _op ) const
        {
            (void)_op;

            return m_value;
        }

    protected:
        const T & m_value;
    };

    template<>
    class AssertionMemoryPanicOperator<void>
    {
    public:
        AssertionMemoryPanicOperator()
        {
        }

    public:
        void operator << ( const AssertionOperator & _op ) const
        {
            (void)_op;
        }
    };

    template<class T>
    AssertionMemoryPanicOperator<T> makeAssertionMemoryPanicOperator( const T & _value )
    {
        return AssertionMemoryPanicOperator<T>( _value );
    }
}

#ifndef NDEBUG
#   define MENGINE_ASSERTION_MEMORY_PANIC( memory, Ret ) if( memory == nullptr ) return makeAssertionMemoryPanicOperator(Ret) << AssertionOperator( "memory == nullptr", __FILE__, __LINE__ )
#   define MENGINE_ASSERTION_MEMORY_PANIC_VOID( memory ) if( memory == nullptr ) return AssertionMemoryPanicOperator<void>() << AssertionOperator( "memory == nullptr", __FILE__, __LINE__ )
#else
#   define MENGINE_ASSERTION_MEMORY_PANIC( memory, Ret )
#endif