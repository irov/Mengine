#pragma once

#include "Interface/LoggerInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class LoggerBase
        : public LoggerInterface
        , public Factorable
    {
    public:
        LoggerBase();
        ~LoggerBase() override;

    private:
        bool initializeLogger() override;
        void finalizeLogger() override;

    protected:
        virtual bool _initializeLogger();
        virtual void _finalizeLogger();

    public:
        void setWriteHistory( bool _writeHistory ) override;
        bool getWriteHistory() const override;

    public:
        void setColor( uint32_t _color ) override;
        uint32_t getColor() const override;

    public:
        void setVerboseLevel( ELoggerLevel _level ) override;
        ELoggerLevel getVerboseLevel() const override;

        void setVerboseFilter( uint32_t _filter ) override;
        uint32_t getVerboseFilter() const override;

    public:
        bool validMessage( const LoggerMessage & _message ) const override;

    public:
        void flush() override;

    protected:
        bool m_writeHistory;

        uint32_t m_color;

        ELoggerLevel m_verboseLevel;
        uint32_t m_verboseFilter;
    };
}
