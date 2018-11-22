#pragma once

#include "Config/Typedef.h"
#include "Config/Char.h"

namespace Mengine
{
    class AssertionOperator
    {
    public:
        AssertionOperator( const Char * _test, const Char * _file, uint32_t _line );
        ~AssertionOperator();

    public:
        AssertionOperator & operator()( const Char * _format, ... );

    protected:
        const Char * m_test;
        const Char * m_file;
        uint32_t m_line;
    };

    void Assertion( const Char * _test, const Char * _file, int _line, const Char * _info );
}
//////////////////////////////////////////////////////////////////////////
#ifndef NDEBUG
#   define MENGINE_ASSERTION(Condition, Message) if(!(Condition)) AssertionOperator( #Condition, __FILE__, __LINE__ ) Message
#else
#   define MENGINE_ASSERTION(Condition, Message)
#endif
//////////////////////////////////////////////////////////////////////////
