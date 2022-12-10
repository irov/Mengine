#pragma once

#include "Interface/ServiceInterface.h"

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
        virtual uint64_t getScriptThreadId() const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define SCRIPTPROVIDER_SERVICE()\
    ((Mengine::ScriptProviderServiceInterface*)SERVICE_GET(Mengine::ScriptProviderServiceInterface))
//////////////////////////////////////////////////////////////////////////