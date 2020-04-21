#pragma once

#include "Kernel/PluginBase.h"

#include "sentry.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SentryPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "Sentry" )

    public:
        SentryPlugin();
        ~SentryPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;

    protected:
        void notifyAssertion_( uint32_t _level, const Char * _test, const Char * _file, int32_t _line, const Char * _message );
        void notifyCreateApplication_();

    protected:
        sentry_options_t * m_options;
    };
}