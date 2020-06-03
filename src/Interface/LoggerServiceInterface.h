#pragma once

#include "Config/Typedef.h"

#include "Interface/ServiceInterface.h"
#include "Interface/LoggerInterface.h"

namespace Mengine
{
    class LoggerServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "LoggerService" )

    public:
        virtual void setVerboseLevel( ELoggerLevel _level ) = 0;
        virtual ELoggerLevel getVerboseLevel() const = 0;

        virtual void setVerboseFlag( uint32_t _flag ) = 0;
        virtual uint32_t getVerboseFlag() const = 0;

    public:
        virtual void setSilent( bool _silent ) = 0;
        virtual bool isSilent() const = 0;

    public:
        virtual int32_t makeTimeStamp( Char * _buffer, int32_t _offset, size_t _capacity ) const = 0;
        virtual int32_t makeFunctionStamp( const Char * _file, uint32_t _line, Char * const _buffer, int32_t _offset, size_t _capacity ) const = 0;

    public:
        virtual bool validMessage( ELoggerLevel _level, uint32_t _flag ) const = 0;

    public:
        virtual void logMessage( ELoggerLevel _level, uint32_t _flag, uint32_t _color, const Char * _message, size_t _size ) = 0;
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

