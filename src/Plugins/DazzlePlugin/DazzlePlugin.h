#pragma once

#include "DazzleIncluder.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class DazzlePlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "Dazzle" )

    public:
        DazzlePlugin();
        ~DazzlePlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        void _destroyPlugin() override;

    protected:
        dz_service_t * m_service;
    };
}