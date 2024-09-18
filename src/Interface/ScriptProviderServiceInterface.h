#pragma once

#include "Interface/ServiceInterface.h"

#include "Config/Thread.h"

//////////////////////////////////////////////////////////////////////////
namespace pybind
{
    class kernel_interface;
}
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    class ScriptProviderServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "ScriptProviderService" )

    public:
        virtual pybind::kernel_interface * getKernel() const = 0;

    public:
        virtual void updateScriptThread() = 0;

    public:
        virtual ThreadId getScriptThreadId() const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define SCRIPTPROVIDER_SERVICE()\
    ((Mengine::ScriptProviderServiceInterface *)SERVICE_GET(Mengine::ScriptProviderServiceInterface))
//////////////////////////////////////////////////////////////////////////