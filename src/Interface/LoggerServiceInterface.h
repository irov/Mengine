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
        virtual void setVerboseLevel( EMessageLevel _level ) = 0;
        virtual void setVerboseFlag( uint32_t _flag ) = 0;

    public:
        virtual bool validMessage( EMessageLevel _level, uint32_t _flag ) const = 0;

    public:
        virtual void logMessage( EMessageLevel _level, uint32_t _flag, const Char * _message, uint32_t _size ) = 0;
        virtual uint32_t getCountMessage( EMessageLevel _level ) = 0;

    public:
        virtual bool registerLogger( const LoggerInterfacePtr & _logger ) = 0;
        virtual bool unregisterLogger( const LoggerInterfacePtr & _logger ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define LOGGER_SERVICE()\
    ((Mengine::LoggerServiceInterface*)SERVICE_GET(Mengine::LoggerServiceInterface))
//////////////////////////////////////////////////////////////////////////

