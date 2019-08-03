#pragma once

#include "Interface/LoggerServiceInterface.h"
#include "Interface/LoggerInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Kernel/ServiceBase.h"

#include "Config/Vector.h"
#include "Config/String.h"

namespace Mengine
{
    class LoggerService
        : public ServiceBase<LoggerServiceInterface>
    {
    public:
        LoggerService();
        ~LoggerService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void setVerboseLevel( ELoggerLevel _level ) override;
        ELoggerLevel getVerboseLevel() const override;

    public:
        void setVerboseFlag( uint32_t _flag ) override;
        uint32_t getVerboseFlag() const override;

    public:
        void setSilent( bool _silent ) override;
        bool isSilent() const override;

    public:
        bool validMessage( ELoggerLevel _level, uint32_t _flag ) const override;

    public:
        void logMessage( ELoggerLevel _level, uint32_t _flag, const Char * _message, uint32_t _size ) override;
        uint32_t getCountMessage( ELoggerLevel _level ) override;

    public:
        void writeHistory( const LoggerInterfacePtr & _logger ) const override;

    public:
        bool registerLogger( const LoggerInterfacePtr & _logger ) override;
        bool unregisterLogger( const LoggerInterfacePtr & _logger ) override;

    protected:
        ELoggerLevel m_verboseLevel;
        uint32_t m_verboseFlag;
        bool m_silent;

        typedef Vector<LoggerInterfacePtr> VectorLoggers;
        VectorLoggers m_loggers;

        ThreadMutexInterfacePtr m_threadMutex;

        uint32_t m_countMessage[LM_MAX];

#ifdef MENGINE_LOGGER_HISTORY
        struct Record
        {
            ELoggerLevel level;
            uint32_t flag;
            String message;
        };

        typedef Vector<Record> VectorHistory;
        VectorHistory m_history;
#endif
    };
}