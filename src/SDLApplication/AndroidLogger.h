#pragma once

#include "Core/ServantBase.h"
#include "Core/LoggerBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidLogger
        : public LoggerBase
    {
    public:
        AndroidLogger();
        ~AndroidLogger();

    public:
        void log( EMessageLevel _level, uint32_t _flag, const Char * _data, size_t _count ) override;
        void flush() override;
    };
}
