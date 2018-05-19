#pragma once

#include "Core/ServantBase.h"
#include "Core/LoggerBase.h"

namespace Mengine
{
    class SDLMessageBoxLogger
        : public LoggerBase
    {
    public:
        SDLMessageBoxLogger();
        ~SDLMessageBoxLogger() override;

    public:
        void log( EMessageLevel _level, uint32_t _flag, const char * _data, size_t _count ) override;
        void flush() override;
    };
}