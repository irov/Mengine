#pragma once

#include "Interface/Interface.h"

#include "Kernel/LoggerMessage.h"
#include "Kernel/LoggerLevel.h"
#include "Kernel/ConstString.h"
#include "Kernel/DateTime.h"

#include "Config/Typedef.h"
#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class LoggerInterface
        : public Interface
    {
    public:
        virtual bool initializeLogger() = 0;
        virtual void finalizeLogger() = 0;

    public:
        virtual void setWriteHistory( bool _writeHistory ) = 0;
        virtual bool getWriteHistory() const = 0;

    public:
        virtual void setVerboseLevel( ELoggerLevel _level ) = 0;
        virtual ELoggerLevel getVerboseLevel() const = 0;

        virtual void setVerboseFilter( uint32_t _filter ) = 0;
        virtual uint32_t getVerboseFilter() const = 0;

    public:
        virtual bool validMessage( const LoggerMessage & _message ) const = 0;

    public:
        virtual void log( const LoggerMessage & _message ) = 0;
        virtual void flush() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<LoggerInterface> LoggerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}