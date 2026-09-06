#pragma once

#include "Kernel/PluginBase.h"

#include "Config/Path.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class XlsExportPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "XlsExport" );

    public:
        XlsExportPlugin();
        ~XlsExportPlugin() override;

    protected:
        const ServiceRequiredList & requiredServices() const override;

    protected:
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        void notifyBootstrapperCreateApplication_();
        void notifyReloadLocale_();

    protected:
        bool process_();

    protected:
        Path m_python3ExecutablePath = {'\0'};
    };
    //////////////////////////////////////////////////////////////////////////
}
