#pragma once

#include "Interface/LoggerServiceInterface.h"
#include "Interface/LoggerInterface.h"
#include "Interface/ThreadSharedMutexInterface.h"
#include "Interface/ThreadConditionVariableInterface.h"
#include "Interface/ThreadIdentityInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"
#include "Kernel/List.h"
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
        void _stopService() override;

    public:
        void setVerboseLevel( ELoggerLevel _level ) override;
        ELoggerLevel getVerboseLevel() const override;

    public:
        void setVerboseFilter( uint32_t _filter ) override;
        uint32_t getVerboseFilter() const override;

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
        void lockMessage() override;
        void unlockMessage() override;

    public:
        void writeHistory( const LoggerInterfacePtr & _logger ) const override;

    public:
        bool loadOldLogMemory( const ContentInterfacePtr & _content ) override;
        const MemoryInterfacePtr & getOldLogMemory() const override;

    public:
        bool registerLogger( const LoggerInterfacePtr & _logger ) override;
        void unregisterLogger( const LoggerInterfacePtr & _logger ) override;

    protected:
        bool hasVerbose( const Char * _category ) const;

    protected:
        void logMessage_( const LoggerMessage & _message );
        void logHistory_( const LoggerMessage & _message );

    protected:
        void notifyConfigsLoad_();

    protected:
        void processMessages_( const ThreadIdentityRunnerInterfacePtr & _runner );

    protected:
        ELoggerLevel m_verboseLevel;
        uint32_t m_verboseFilter;
        bool m_silent;
        bool m_silentMessageRelease;

        typedef Vector<ConstString> VectorVerboses;
        VectorVerboses m_verboses;

        typedef Vector<LoggerInterfacePtr> VectorLoggers;
        VectorLoggers m_loggers;

        struct LoggerMessageRecord
        {
            Char category[64];
            PlatformDateTime dateTime;
            ELoggerLevel level;
            uint32_t flag;
            uint32_t filter;
            uint32_t color;
            const Char * file;
            int32_t line;
            Char data[MENGINE_LOGGER_MAX_MESSAGE];
            size_t size;
        };

        typedef Vector<LoggerMessageRecord> VectorLoggerMessageRecords;
        VectorLoggerMessageRecords m_messages;
        VectorLoggerMessageRecords m_messagesAux;

        ThreadSharedMutexInterfacePtr m_mutexMessage;
        ThreadSharedMutexInterfacePtr m_mutexLogger;
        ThreadSharedMutexInterfacePtr m_mutexHistory;

        ThreadConditionVariableInterfacePtr m_conditionLogger;
        ThreadIdentityInterfacePtr m_threadLogger;
        ThreadIdentityRunnerInterfacePtr m_threadRunner;

        ThreadMutexInterfacePtr m_mutexMessageBlock;

        uint32_t m_staticsticLevel[LOGGER_LEVEL_COUNT];

        bool m_historically;

        uint32_t m_historyLimit;

        MemoryInterfacePtr m_memoryOldLog;

        struct HistoryRecord
        {
            const Char * category;
            PlatformDateTime dateTime;
            ELoggerLevel level;
            uint32_t flag;
            uint32_t filter;
            uint32_t color;
            const Char * file;
            int32_t line;
            VectorChar data;
        };

        typedef List<HistoryRecord> ListHistoryRecords;
        ListHistoryRecords m_history;
    };
}
