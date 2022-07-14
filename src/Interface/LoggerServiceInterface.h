#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/LoggerInterface.h"

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
        virtual size_t makeTimeStamp( Char * const _buffer, size_t _offset, size_t _capacity ) const = 0;
        virtual size_t makeFunctionStamp( const Char * _file, uint32_t _line, Char * const _buffer, size_t _offset, size_t _capacity ) const = 0;

    public:
        virtual bool validMessage( const ConstString & _category, ELoggerLevel _level, uint32_t _filter ) const = 0;

    public:
        virtual void logMessage( ELoggerLevel _level, uint32_t _filter, uint32_t _color, const Char * _message, size_t _size ) = 0;
        virtual uint32_t getCountMessage( ELoggerLevel _level ) = 0;

    public:
        virtual void writeHistory( const LoggerInterfacePtr & _logger ) const = 0;

    public:
        virtual bool registerLogger( const LoggerInterfacePtr & _logger ) = 0;
        virtual bool unregisterLogger( const LoggerInterfacePtr & _logger ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define LOGGER_SERVICE()\
    ((Mengine::LoggerServiceInterface*)SERVICE_GET(Mengine::LoggerServiceInterface))
//////////////////////////////////////////////////////////////////////////

