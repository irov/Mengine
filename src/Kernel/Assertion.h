#pragma once

#include "Config/Config.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)

#include "Kernel/AssertionLevel.h"

#include "Config/Typedef.h"
#include "Config/Char.h"
#include "Config/StdIO.h"

#ifndef MENGINE_ASSERTION_MAX_MESSAGE
#define MENGINE_ASSERTION_MAX_MESSAGE 8192
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void AssertionSetNotDebugBreak( bool _debugBreak );
        bool AssertionIsNotDebugBreak();
        void AssertionMessage( const Char * _category, const Char * _file, int32_t _line, const Char * _function, const Char * _message );
        //////////////////////////////////////////////////////////////////////////
        void Assertion( const Char * _category, EAssertionLevel _level, const Char * _test, const Char * _file, int32_t _line, const Char * _function );
        void Assertion( const Char * _category, EAssertionLevel _level, const Char * _test, const Char * _file, int32_t _line, const Char * _function, const Char * _format, ... ) MENGINE_ATTRIBUTE_FORMAT_STRING( 7, 8 );
        //////////////////////////////////////////////////////////////////////////
        class AssertionOperator
        {
        public:
            AssertionOperator( const Char * _category, EAssertionLevel _level, const Char * _test, const Char * _file, int32_t _line, const Char * _function );
            ~AssertionOperator();

        public:
            const AssertionOperator & operator()() const;
            const AssertionOperator & operator()( const Char * _format, ... ) const MENGINE_ATTRIBUTE_FORMAT_STRING( 2, 3 );

        private:
            template<class T>
            const AssertionOperator & operator()( T, ... ) const = delete;

        protected:
            const Char * m_category;
            EAssertionLevel m_level;
            const Char * m_test;
            const Char * m_file;
            int32_t m_line;
            const Char * m_function;
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
        //////////////////////////////////////////////////////////////////////////
    }
}

#   define MENGINE_ASSERTION_SET_NOT_DEBUG_BREAK( DebugBreak ) Mengine::Helper::AssertionSetNotDebugBreak( DebugBreak )
#   define MENGINE_ASSERTION_IS_NOT_DEBUG_BREAK() Mengine::Helper::AssertionIsNotDebugBreak()

#   define DETAIL__MENGINE_ASSERTION_CALL( ... ) (__VA_ARGS__ ) ;static_assert(sizeof(MENGINE_PRINTF(__VA_ARGS__)))

#   define MENGINE_ASSERTION(Condition, ...) if(!(Condition)) Mengine::Helper::AssertionOperator( MENGINE_CODE_LIBRARY, Mengine::ASSERTION_LEVEL_ERROR, #Condition, MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION ) DETAIL__MENGINE_ASSERTION_CALL(__VA_ARGS__)
#   define MENGINE_ASSERTION_FATAL(Condition, ...) if(!(Condition)) Mengine::Helper::AssertionOperator( MENGINE_CODE_LIBRARY, Mengine::ASSERTION_LEVEL_FATAL, #Condition, MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION ) DETAIL__MENGINE_ASSERTION_CALL(__VA_ARGS__)
#   define MENGINE_ASSERTION_EXCEPTION(Condition, ...) if(!(Condition)) Mengine::Helper::AssertionOperator( MENGINE_CODE_LIBRARY, Mengine::ASSERTION_LEVEL_EXCEPTION, #Condition, MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION ) DETAIL__MENGINE_ASSERTION_CALL(__VA_ARGS__)
#   define MENGINE_ASSERTION_CRITICAL(Condition, ...) if(!(Condition)) Mengine::Helper::AssertionOperator( MENGINE_CODE_LIBRARY, Mengine::ASSERTION_LEVEL_CRITICAL, #Condition, MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION ) DETAIL__MENGINE_ASSERTION_CALL(__VA_ARGS__)
#else
#   define MENGINE_ASSERTION_SET_NOT_DEBUG_BREAK( DebugBreak ) MENGINE_UNUSED( DebugBreak )
#   define MENGINE_ASSERTION_IS_NOT_DEBUG_BREAK() true

#   define MENGINE_ASSERTION(Condition, ...)
#   define MENGINE_ASSERTION_FATAL(Condition, ...)
#   define MENGINE_ASSERTION_EXCEPTION(Condition, ...)
#   define MENGINE_ASSERTION_CRITICAL(Condition, ...)
#endif
//////////////////////////////////////////////////////////////////////////
