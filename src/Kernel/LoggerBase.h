#pragma once

#include "Interface/LoggerInterface.h"
#include "Interface/ThreadMutexInterface.h"

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

    public:
        bool initializeLogger() override;
        void finalizeLogger() override;

    protected:
        virtual bool _initializeLogger();
        virtual void _finalizeLogger();

    public:
        void setWriteHistory( bool _writeHistory ) override;
        bool getWriteHistory() const override;

    public:
        void setVerboseLevel( ELoggerLevel _level ) override;
        ELoggerLevel getVerboseLevel() const override;

        void setVerboseFilter( uint32_t _filter ) override;
        uint32_t getVerboseFilter() const override;

    public:
        bool validMessage( const LoggerRecordInterfacePtr & _record ) const override;

    public:
        void log( const LoggerRecordInterfacePtr & _record ) override final;
        void flush() override final;

    protected:
        virtual void _log( const LoggerRecordInterfacePtr & _record ) = 0;
        virtual void _flush();

    protected:
        ThreadMutexInterfacePtr m_mutex;

        ELoggerLevel m_verboseLevel;
        uint32_t m_verboseFilter;

        bool m_writeHistory;
    };
}
