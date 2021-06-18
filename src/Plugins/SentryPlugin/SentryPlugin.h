#pragma once

#include "SentryLoggerCapture.h"

#include "Kernel/PluginBase.h"
#include "Kernel/ErrorLevel.h"

namespace Mengine
{    
    class SentryPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "Sentry" )

    public:
        SentryPlugin();
        ~SentryPlugin() override;

    protected:
        bool _unimportantPlugin() const override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        void notifyAssertion_( uint32_t _level, const Char * _test, const Char * _file, int32_t _line, const Char * _message );
        void notifyError_( EErrorLevel _level, const Char * _file, int32_t _line, const Char * _message );
        void notifyCreateApplication_();

    protected:
        SentryLoggerCapturePtr m_loggerCapture;
    };
}