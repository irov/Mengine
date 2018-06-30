#pragma once

#include "Core/ServantBase.h"
#include "Core/LoggerBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SDLStdioLogger
        : public LoggerBase
    {
    public:
		SDLStdioLogger();
        ~SDLStdioLogger();

    public:
        void log( EMessageLevel _level, uint32_t _flag, const Char * _data, uint32_t _count ) override;
        void flush() override;
    };
}
