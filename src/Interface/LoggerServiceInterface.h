#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/LoggerInterface.h"
#include "Interface/ContentInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/ConstString.h"

#include "Config/Typedef.h"

namespace Mengine
{
    class LoggerServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "LoggerService" )

    public:
        virtual void setVerboseLevel( ELoggerLevel _level ) = 0;
        virtual ELoggerLevel getVerboseLevel() const = 0;

        virtual void setVerboseFilter( uint32_t _filter ) = 0;
        virtual uint32_t getVerboseFilter() const = 0;

    public:
        virtual void setThreadMode( ELoggerMode _threadMode ) = 0;
        virtual ELoggerMode getThreadMode() const = 0;

    public:
        virtual void setSilent( bool _silent ) = 0;
        virtual bool isSilent() const = 0;

    public:
        virtual void setSilentMessageRelease( bool _silentMessageRelease ) = 0;
        virtual bool isSilentMessageRelease() const = 0;

    public:
        virtual void setHistorically( bool _historically ) = 0;
        virtual bool getHistorically() const = 0;

        virtual void clearHistory() = 0;

    public:
        virtual bool validMessage( const Char * _category, ELoggerLevel _level, uint32_t _filter ) const = 0;

    public:
        virtual void logMessage( const LoggerMessage & _message ) = 0;

    public:
        virtual void lockMessages() = 0;
        virtual void unlockMessages() = 0;

    public:
        virtual void flushMessages() = 0;

    public:
        virtual bool loadOldLogMemory( const ContentInterfacePtr & _content ) = 0;
        virtual const MemoryInterfacePtr & getOldLogMemory() const = 0;

    public:
        virtual void setCurrentContentLog( const ContentInterfacePtr & _content ) = 0;
        virtual const ContentInterfacePtr & getCurrentContentLog() const = 0;

    public:
        virtual void writeHistory( const LoggerInterfacePtr & _logger ) const = 0;

    public:
        virtual bool registerLogger( const LoggerInterfacePtr & _logger ) = 0;
        virtual void unregisterLogger( const LoggerInterfacePtr & _logger ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define LOGGER_SERVICE()\
    ((Mengine::LoggerServiceInterface *)SERVICE_GET(Mengine::LoggerServiceInterface))
//////////////////////////////////////////////////////////////////////////

