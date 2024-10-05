#pragma once

#include "Interface/LoggerServiceInterface.h"
#include "Interface/LoggerInterface.h"
#include "Interface/ThreadSharedMutexInterface.h"
#include "Interface/ThreadConditionVariableInterface.h"
#include "Interface/ThreadIdentityInterface.h"
#include "Interface/FactoryInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"
#include "Kernel/VectorChar.h"

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
        bool _runService() override;
        void _stopService() override;

    public:
        void setVerboseLevel( ELoggerLevel _level ) override;
        ELoggerLevel getVerboseLevel() const override;

    public:
        void setVerboseFilter( uint32_t _filter ) override;
        uint32_t getVerboseFilter() const override;

    public:
        void setThreadMode( ELoggerMode _threadMode ) override;
        ELoggerMode getThreadMode() const override;

    public:
        void setSilent( bool _silent ) override;
        bool isSilent() const override;

    public:
        void setSilentMessageRelease( bool _silentMessageRelease ) override;
        bool isSilentMessageRelease() const override;

    public:
        void setHistorically( bool _historically ) override;
        bool getHistorically() const override;

    public:
        void clearHistory() override;

    public:
        bool validMessage( const Char * _category, ELoggerLevel _level, uint32_t _filter ) const override;

    public:
        void logMessage( const LoggerMessage & _message ) override;

    public:
        void lockMessages() override;
        void unlockMessages() override;

    public:
        void flushMessages() override;

    public:
        void writeHistory( const LoggerInterfacePtr & _logger ) const override;

    public:
        bool loadOldLogMemory( const ContentInterfacePtr & _content ) override;
        const MemoryInterfacePtr & getOldLogMemory() const override;

    public:
        void setCurrentContentLog( const ContentInterfacePtr & _content ) override;
        const ContentInterfacePtr & getCurrentContentLog() const override;

    public:
        bool registerLogger( const LoggerInterfacePtr & _logger ) override;
        void unregisterLogger( const LoggerInterfacePtr & _logger ) override;

    protected:
        bool hasVerbose( const Char * _category ) const;

    protected:
        LoggerRecordInterfacePtr makeLoggerRecord( const LoggerMessage & _message, const DocumentInterfacePtr & _doc );

    protected:
        void logMessage_( const LoggerRecordInterfacePtr & _record );
        void logHistory_( const LoggerRecordInterfacePtr & _record );

    protected:
        void notifyConfigsLoad_();
        void notifyBootstrapperRunCompete_();

    protected:
        void processMessages_( const ThreadIdentityRunnerInterfacePtr & _runner );

    protected:
        FactoryInterfacePtr m_factoryLoggerRecord;

        ELoggerLevel m_verboseLevel;
        uint32_t m_verboseFilter;
        bool m_silent;
        bool m_silentMessageRelease;

        typedef Vector<ConstString> VectorVerboses;
        VectorVerboses m_verboses;

        typedef Vector<LoggerInterfacePtr> VectorLoggers;
        VectorLoggers m_loggers;

        typedef Vector<LoggerRecordInterfacePtr> VectorLoggerRecords;
        VectorLoggerRecords m_messages;
        VectorLoggerRecords m_messagesAux;

        ThreadSharedMutexInterfacePtr m_mutexMessage;
        ThreadSharedMutexInterfacePtr m_mutexLogger;
        ThreadSharedMutexInterfacePtr m_mutexHistory;

        ThreadConditionVariableInterfacePtr m_conditionLogger;
        ThreadIdentityInterfacePtr m_threadLogger;
        ThreadIdentityRunnerInterfacePtr m_threadRunner;

        ThreadMutexInterfacePtr m_mutexMessageBlock;

        uint32_t m_staticsticLevel[LOGGER_LEVEL_COUNT];

        bool m_historically;
        bool m_threadly;
        ELoggerMode m_threadMode;

        MemoryInterfacePtr m_memoryOldLog;
        ContentInterfacePtr m_currentContentLog;

        VectorLoggerRecords m_history;
    };
}
