#pragma once

#include "Kernel/ErrorLevel.h"

#include "Config/Typedef.h"
#include "Config/Char.h"

namespace Mengine
{
    class ErrorOperator
    {
    public:
        ErrorOperator( EErrorLevel _level, const Char * _file, uint32_t _line );
        ~ErrorOperator();

    public:
        void operator()( MENGINE_CHECK_FORMAT_STRING( const Char * _format ), ... ) const MENGINE_ATTRIBUTE_FORMAT_STRING( 2, 3 );

    protected:
        EErrorLevel m_level;
        const Char * m_file;
        uint32_t m_line;
    };
}
//////////////////////////////////////////////////////////////////////////
#define MENGINE_ERROR_MESSAGE(...) Mengine::ErrorOperator( ERROR_LEVEL_MESSAGE, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE )(__VA_ARGS__)
#define MENGINE_ERROR_FATAL(...) Mengine::ErrorOperator( ERROR_LEVEL_FATAL, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE )(__VA_ARGS__)
//////////////////////////////////////////////////////////////////////////