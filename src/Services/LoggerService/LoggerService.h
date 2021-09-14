#pragma once

#include "Interface/LoggerServiceInterface.h"
#include "Interface/LoggerInterface.h"
#include "Interface/ThreadMutexInterface.h"
#include "Interface/DateTimeProviderInterface.h"

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
        void setVerboseFlag( uint32_t _flag ) override;
        uint32_t getVerboseFlag() const override;

    public:
        void setSilent( bool _silent ) override;
        bool isSilent() const override;

    public:
        void setHistorically( bool _historically ) override;
        bool getHistorically() const override;

        void clearHistory() override;

    public:
        size_t makeTimeStamp( Char * const _buffer, size_t _offset, size_t _capacity ) const override;
        size_t makeFunctionStamp( const Char * _file, uint32_t _line, Char * const _buffer, size_t _offset, size_t _capacity ) const override;

    public:
        bool validMessage( const ConstString & _category, ELoggerLevel _level, uint32_t _flag ) const override;

    public:
        void logMessage( ELoggerLevel _level, uint32_t _flag, uint32_t _color, const Char * _message, size_t _size ) override;
        uint32_t getCountMessage( ELoggerLevel _level ) override;

    public:
        void writeHistory( const LoggerInterfacePtr & _logger ) const override;

    public:
        bool registerLogger( const LoggerInterfacePtr & _logger ) override;
        bool unregisterLogger( const LoggerInterfacePtr & _logger ) override;

    protected:
        void logHistory_( ELoggerLevel _level, uint32_t _flag, uint32_t _color, const Char * _message, size_t _size );

    protected:
        void notifyConfigsLoad_();

    protected:
        DateTimeProviderInterfacePtr m_dateTimeProvider;

        ELoggerLevel m_verboseLevel;
        uint32_t m_verboseFlag;
        bool m_silent;

        typedef Vector<ConstString> VectorVerboses;
        VectorVerboses m_verboses;

        typedef Vector<LoggerInterfacePtr> VectorLoggers;
        VectorLoggers m_loggers;

        ThreadMutexInterfacePtr m_mutex;

        uint32_t m_countMessage[__LM_MAX__] = {0};

        struct Record
        {
            ELoggerLevel level;
            uint32_t flag;
            uint32_t color;
            String message;
        };

        bool m_historically;

        uint32_t m_historyLimit;

        typedef List<Record> ListHistory;
        ListHistory m_history;
    };
}