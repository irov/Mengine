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
    class AssertionOperator
    {
    public:
        AssertionOperator( uint32_t _level, const Char * _test, const Char * _file, uint32_t _line );
        ~AssertionOperator();

    public:
        const AssertionOperator & operator()( const Char * _format, ... ) const;

    protected:
        uint32_t m_level;
        const Char * m_test;
        const Char * m_file;
        uint32_t m_line;
    };

    void Assertion( uint32_t _level, const Char * _test, const Char * _file, int32_t _line, const Char * _info );
}
//////////////////////////////////////////////////////////////////////////
#ifndef NDEBUG
#   define MENGINE_ASSERTION(Condition, Message) if(!(Condition)) AssertionOperator( ASSERTION_LEVEL_ERROR, #Condition, __FILE__, __LINE__ ) Message
#   define MENGINE_ASSERTION_FATAL(Condition, Message) if(!(Condition)) AssertionOperator( ASSERTION_LEVEL_FATAL, #Condition, __FILE__, __LINE__ ) Message
#else
#   define MENGINE_ASSERTION(Condition, Message)
#   define MENGINE_ASSERTION_FATAL(Condition, Message) if(!(Condition)) AssertionOperator( ASSERTION_LEVEL_FATAL, #Condition, __FILE__, __LINE__ ) Message
#endif
//////////////////////////////////////////////////////////////////////////
