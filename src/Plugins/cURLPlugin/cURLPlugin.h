#pragma once

#include "cURLInterface.h"

#include "Kernel/PluginBase.h"
#include "Kernel/Factory.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    class cURLPlugin
        : public PluginBase
    {
    public:
        PLUGIN_DECLARE( "cURL" );

    public:
        cURLPlugin();
        ~cURLPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        void _destroy() override;
    };
}

