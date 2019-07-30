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
        void setVerboseLevel( ELoggerLevel _level ) override;
        ELoggerLevel getVerboseLevel() const override;

        void setVerboseFlag( uint32_t _flag ) override;
        uint32_t getVerboseFlag() const override;

    public:
        bool validMessage( ELoggerLevel _level, uint32_t _flag ) const override;

    public:
        void flush() override;

    protected:
        ELoggerLevel m_verboseLevel;
        uint32_t m_verboseFlag;
    };
}
