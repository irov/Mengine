#pragma once

#include "Interface/LoggerInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class LoggerBase
        : public LoggerInterface
        , public Factorable
    {
    public:
        LoggerBase();
        ~LoggerBase() override;

    public:
        bool initialize() override;
        void finalize() override;

    public:
        void setVerboseLevel( EMessageLevel _level ) override;
        void setVerboseFlag( uint32_t _flag ) override;

    public:
        bool validMessage( EMessageLevel _level, uint32_t _flag ) const override;

    public:
        void flush() override;

    protected:
        EMessageLevel m_verboseLevel;
        uint32_t m_verboseFlag;
    };
}
