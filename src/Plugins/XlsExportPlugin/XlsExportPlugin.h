#pragma once

#include "Kernel/PluginBase.h"
#include "Kernel/ConstString.h"

#include "XlsScriptLogger.h"

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
        void notifyReloadLocale();

    protected:
        bool proccess_();

    protected:
        void warning_( const WChar * _msg );
        void error_( const WChar * _msg );

    protected:
        XlsScriptLogger * m_warninglogger;
        XlsScriptLogger * m_errorLogger;
    };
}