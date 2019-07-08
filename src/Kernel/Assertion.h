#pragma once

#include "Config/Typedef.h"
#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EAssertionLevel
    {
        ASSERTION_LEVEL_FATAL = 0,
        ASSERTION_LEVEL_ERROR = 2,
        ASSERTION_LEVEL_WARNING = 3
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////    
        void Assertion( uint32_t _level, const Char * _test, const Char * _file, int32_t _line, const Char * _format, ... );
        //////////////////////////////////////////////////////////////////////////
        class AssertionOperator
        {
        public:
            AssertionOperator( uint32_t _level, const Char * _test, const Char * _file, uint32_t _line );
            ~AssertionOperator();

        public:
            const AssertionOperator & operator()() const;
            const AssertionOperator & operator()( const Char * _format, ... ) const;

        protected:
            uint32_t m_level;
            const Char * m_test;
            const Char * m_file;
            uint32_t m_line;
        };
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        class AssertionReturnOperator
        {
        public:
            AssertionReturnOperator( const T & _value )
                : m_value( _value )
            {
            }

            ~AssertionReturnOperator()
            {
            }

        public:
            const T & operator << ( const AssertionOperator & _op ) const
            {
                MENGINE_UNUSED( _op );

                return m_value;
            }

        protected:
            const T & m_value;
        };
        //////////////////////////////////////////////////////////////////////////
        template<>
        class AssertionReturnOperator<void>
        {
        public:
            AssertionReturnOperator()
            {
            }

            ~AssertionReturnOperator()
            {
            }

        public:
            void operator << ( const AssertionOperator & _op ) const
            {
                MENGINE_UNUSED( _op );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        AssertionReturnOperator<T> makeAssertionReturnOperator( const T & _value )
        {
            return AssertionReturnOperator<T>( _value );
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE AssertionReturnOperator<void> makeAssertionReturnOperator()
        {
            return AssertionReturnOperator<void>();
        }
    }
}
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_DEBUG
#   define MENGINE_ASSERTION(Condition, ...) if(!(Condition)) Helper::AssertionOperator( ASSERTION_LEVEL_ERROR, #Condition, MENGINE_CODE_FILE, MENGINE_CODE_LINE ) (__VA_ARGS__)
#   define MENGINE_ASSERTION_RETURN(Condition, Ret, ...) if(!(Condition)) return Helper::makeAssertionReturnOperator(Ret) << Helper::AssertionOperator( ASSERTION_LEVEL_ERROR, #Condition, MENGINE_CODE_FILE, MENGINE_CODE_LINE ) (__VA_ARGS__)
#   define MENGINE_ASSERTION_RETURN_VOID(Condition, ...) if(!(Condition)) return Helper::makeAssertionReturnOperator() << Helper::AssertionOperator( ASSERTION_LEVEL_ERROR, #Condition, MENGINE_CODE_FILE, MENGINE_CODE_LINE ) (__VA_ARGS__)
#   define MENGINE_ASSERTION_FATAL(Condition, ...) if(!(Condition)) Helper::AssertionOperator( ASSERTION_LEVEL_FATAL, #Condition, MENGINE_CODE_FILE, MENGINE_CODE_LINE ) (__VA_ARGS__)
#else
#   define MENGINE_ASSERTION(Condition, ...)
#   define MENGINE_ASSERTION_RETURN(Condition, Ret, ...)
#   define MENGINE_ASSERTION_RETURN_VOID(Condition, ...)
#   define MENGINE_ASSERTION_FATAL(Condition, ...)
#endif
//////////////////////////////////////////////////////////////////////////
