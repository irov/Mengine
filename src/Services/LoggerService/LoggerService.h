#pragma once

#include "Interface/LoggerServiceInterface.h"
#include "Interface/LoggerInterface.h"
#include "Interface/ThreadMutexInterface.h"
#include "Interface/DateTimeProviderInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"
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
        int32_t makeTimeStamp( Char * _buffer, int32_t _offset, size_t _capacity ) const override;
        int32_t makeFunctionStamp( const Char * _file, uint32_t _line, Char * _buffer, int32_t _offset, size_t _capacity ) const override;

    public:
        bool validMessage( ELoggerLevel _level, uint32_t _flag ) const override;

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
        DateTimeProviderInterfacePtr m_dateTimeProvider;

        ELoggerLevel m_verboseLevel;
        uint32_t m_verboseFlag;
        bool m_silent;

        typedef Vector<LoggerInterfacePtr> VectorLoggers;
        VectorLoggers m_loggers;

        ThreadMutexInterfacePtr m_threadMutex;

        uint32_t m_countMessage[LM_MAX];

        struct Record
        {
            ELoggerLevel level;
            uint32_t flag;
            uint32_t color;
            String message;
        };

        bool m_historically;

        typedef Vector<Record> VectorHistory;
        VectorHistory m_history;
    };
}