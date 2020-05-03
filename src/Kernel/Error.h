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
        void operator()( MENGINE_CHECK_FORMAT_STRING( const Char * _format ), ... ) const;

    protected:
        const Char * m_file;
        uint32_t m_line;
    };
}
//////////////////////////////////////////////////////////////////////////
#define MENGINE_ERROR_FATAL(Format, ...) Mengine::ErrorOperator( MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE )(Format, __VA_ARGS__)
//////////////////////////////////////////////////////////////////////////