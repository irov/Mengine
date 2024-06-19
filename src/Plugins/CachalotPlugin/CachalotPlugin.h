#pragma once

#include "Interface/LoggerInterface.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{    
    class CachalotPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "Cachalot" )

    public:
        CachalotPlugin();
        ~CachalotPlugin() override;

    protected:
        bool _unimportantPlugin() const override;

    protected:
        const ServiceRequiredList & requiredServices() const override;

    protected:
        bool _availablePlugin() const override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        LoggerInterfacePtr m_loggerCachalot;
    };
}