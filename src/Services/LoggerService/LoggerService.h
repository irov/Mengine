#pragma once

#include "Interface/LoggerServiceInterface.h"
#include "Interface/LoggerInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"
#include "Kernel/List.h"
#include "Kernel/String.h"

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
        bool validMessage( const ConstString & _category, ELoggerLevel _level, uint32_t _filter ) const override;

    public:
        void logMessage( const LoggerMessage & _message ) override;
        uint32_t getCountMessage( ELoggerLevel _level ) const override;

    public:
        void lockMessage() override;
        void unlockMessage() override;

    public:
        void writeHistory( const LoggerInterfacePtr & _logger ) const override;

    public:
        bool registerLogger( const LoggerInterfacePtr & _logger ) override;
        bool unregisterLogger( const LoggerInterfacePtr & _logger ) override;

    protected:
        void logHistory_( const LoggerMessage & _message );

    protected:
        void notifyConfigsLoad_();

    protected:
        ELoggerLevel m_verboseLevel;
        uint32_t m_verboseFilter;
        bool m_silent;
        bool m_silentMessageRelease;

        typedef Vector<ConstString> VectorVerboses;
        VectorVerboses m_verboses;

        typedef Vector<LoggerInterfacePtr> VectorLoggers;
        VectorLoggers m_loggers;

        ThreadMutexInterfacePtr m_mutex;

        uint32_t m_countMessage[LOGGER_LEVEL_COUNT];

        struct HistoryRecord
        {
            ConstString category;
            PlatformDateTime dateTime;
            ELoggerLevel level;
            ELoggerFlag flag;
            uint32_t filter;
            uint32_t color;
            const Char * file;
            int32_t line;
            String data;
        };

        bool m_historically;

        uint32_t m_historyLimit;

        typedef List<HistoryRecord> ListHistoryRecords;
        ListHistoryRecords m_history;
    };
}
