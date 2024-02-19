#pragma once

#include "Interface/ServantInterface.h"

#include "Kernel/LoggerMessage.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class LoggerRecordInterface
        : public ServantInterface
    {
    public:
        virtual void initialize( const LoggerMessage & _message ) = 0;

    public:
        virtual void getMessage( LoggerMessage * const _message ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<LoggerRecordInterface> LoggerRecordInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}