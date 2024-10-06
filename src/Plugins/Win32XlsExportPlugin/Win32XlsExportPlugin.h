#pragma once

#include "Kernel/PluginBase.h"
#include "Kernel/ConstString.h"

namespace Mengine
{
    class Win32XlsExportPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "Win32XlsExport" )

    public:
        Win32XlsExportPlugin();
        ~Win32XlsExportPlugin() override;

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
        Char m_python3Path[MENGINE_MAX_PATH + 1] = {L'\0'};
    };
}