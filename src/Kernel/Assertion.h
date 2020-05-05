#pragma once

#include "Config/Typedef.h"
#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EAssertionLevel
    {
        ASSERTION_LEVEL_WARNING = 1,
        ASSERTION_LEVEL_ERROR = 2,
        ASSERTION_LEVEL_FATAL = 3,
        ASSERTION_LEVEL_EXCEPTION = 4,
        ASSERTION_LEVEL_CRITICAL = 5
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void Assertion( uint32_t _level, const Char * _test, const Char * _file, int32_t _line );
        void Assertion( uint32_t _level, const Char * _test, const Char * _file, int32_t _line, MENGINE_CHECK_FORMAT_STRING( const Char * _format ), ... ) MENGINE_ATTRIBUTE_FORMAT_STRING( 5, 6 );
        //////////////////////////////////////////////////////////////////////////
        class AssertionOperator
        {
        public:
            AssertionOperator( uint32_t _level, const Char * _test, const Char * _file, uint32_t _line );
            ~AssertionOperator();

        public:
            const AssertionOperator & operator()() const;
            const AssertionOperator & operator()( MENGINE_CHECK_FORMAT_STRING( const Char * _format ), ... ) const MENGINE_ATTRIBUTE_FORMAT_STRING( 2, 3 );

        private:
            const AssertionOperator & operator()( std::nullptr_t ) const = delete;
            const AssertionOperator & operator()( bool ) const = delete;

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
#ifdef MENGINE_ASSERTION_DEBUG
#   define MENGINE_ASSERTION(Condition, ...) if(!(Condition)) Mengine::Helper::AssertionOperator( Mengine::ASSERTION_LEVEL_ERROR, #Condition, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE ) (__VA_ARGS__)
#   define MENGINE_ASSERTION_RETURN(Condition, Ret, ...) if(!(Condition)) return Mengine::Helper::makeAssertionReturnOperator(Ret) << Helper::AssertionOperator( Mengine::ASSERTION_LEVEL_ERROR, #Condition, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE ) (__VA_ARGS__)
#   define MENGINE_ASSERTION_FATAL(Condition, ...) if(!(Condition)) Mengine::Helper::AssertionOperator( Mengine::ASSERTION_LEVEL_FATAL, #Condition, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE ) (__VA_ARGS__)
#   define MENGINE_ASSERTION_FATAL_RETURN(Condition, Ret, ...) if(!(Condition)) return Mengine::Helper::makeAssertionReturnOperator(Ret) << Helper::AssertionOperator( Mengine::ASSERTION_LEVEL_FATAL, #Condition, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE ) (__VA_ARGS__)
#   define MENGINE_ASSERTION_EXCEPTION(Condition, ...) if(!(Condition)) Mengine::Helper::AssertionOperator( Mengine::ASSERTION_LEVEL_EXCEPTION, #Condition, nullptr, 0 ) (__VA_ARGS__)
#   define MENGINE_ASSERTION_CRITICAL(Condition, ...) if(!(Condition)) Mengine::Helper::AssertionOperator( Mengine::ASSERTION_LEVEL_CRITICAL, #Condition, nullptr, 0 ) (__VA_ARGS__)
#   define MENGINE_ASSERTION_RESULT(Result, ...) MENGINE_ASSERTION(Result, __VA_ARGS__)
#   define MENGINE_ASSERTION_RESULT_FATAL(Result, ...) MENGINE_ASSERTION_FATAL(Result, __VA_ARGS__)
#else
#   define MENGINE_ASSERTION(Condition, ...)
#   define MENGINE_ASSERTION_RETURN(Condition, Ret, ...)
#   define MENGINE_ASSERTION_FATAL(Condition, ...)
#   define MENGINE_ASSERTION_FATAL_RETURN(Condition, Ret, ...)
#   define MENGINE_ASSERTION_EXCEPTION(Condition, ...)
#   define MENGINE_ASSERTION_CRITICAL(Condition, ...)
#   define MENGINE_ASSERTION_RESULT(Result, ...) MENGINE_UNUSED(Result)
#   define MENGINE_ASSERTION_RESULT_FATAL(Result, ...) MENGINE_UNUSED(Result)
#endif
//////////////////////////////////////////////////////////////////////////
