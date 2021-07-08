#pragma once

#include "Config/Char.h"
#include "Config/Typedef.h"

namespace Mengine
{
    class ExceptionHelper
    {
    public:
        ExceptionHelper( const Char * _file, uint32_t _line ) noexcept;
        ~ExceptionHelper() noexcept;

    public:
        void operator () ( const Char * _format, ... ) const;

    protected:
        const Char * m_file;
        uint32_t m_line;
    };
}
//////////////////////////////////////////////////////////////////////////
#define MENGINE_THROW_EXCEPTION Mengine::ExceptionHelper(MENGINE_CODE_FILE, MENGINE_CODE_LINE)
#define MENGINE_THROW_EXCEPTION_FL Mengine::ExceptionHelper
//////////////////////////////////////////////////////////////////////////

