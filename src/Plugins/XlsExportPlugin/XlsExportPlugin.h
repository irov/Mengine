#pragma once

#include "Kernel/PluginBase.h"
#include "Kernel/ConstString.h"

namespace Mengine
{
    class XlsExportPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "XlsExport" )

    public:
        XlsExportPlugin();
        ~XlsExportPlugin() override;

    protected:
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        void notifyBootstrapperCreateApplication_();
        void notifyReloadLocale_();

    protected:
        bool proccess_();

    protected:
        bool findPython3Path_();

    protected:
        Char m_python3Path[MENGINE_MAX_PATH] = {L'\0'};
    };
}