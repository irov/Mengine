#pragma once

#include "Config/Typedef.h"
#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ErrorOperator
    {
    public:
        ErrorOperator( const Char * _file, uint32_t _line );
        ~ErrorOperator();

    public:
        void operator()( const Char * _format, ... ) const;

    protected:
        const Char * m_file;
        uint32_t m_line;
    };
}
//////////////////////////////////////////////////////////////////////////
#define ERROR_FATAL\
	Mengine::ErrorOperator( MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE )
//////////////////////////////////////////////////////////////////////////