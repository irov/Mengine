#pragma once

#include "Config/Char.h"
#include "Config/Typedef.h"

namespace Mengine
{
    class ExceptionHelper
    {
    public:
        ExceptionHelper( const Char * _file, int32_t _line, const Char * _function );
        ~ExceptionHelper();

    public:
        void operator () ( const Char * _format, ... ) const;

    protected:
        const Char * m_file;
        int32_t m_line;
        const Char * m_function;
    };
}
//////////////////////////////////////////////////////////////////////////
#define MENGINE_THROW_EXCEPTION Mengine::ExceptionHelper(MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION)
#define MENGINE_THROW_EXCEPTION_FL Mengine::ExceptionHelper
//////////////////////////////////////////////////////////////////////////

