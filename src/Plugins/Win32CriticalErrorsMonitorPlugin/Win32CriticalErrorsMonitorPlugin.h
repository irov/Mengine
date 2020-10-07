#pragma once

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32CriticalErrorsMonitorPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "Win32CriticalErrorsMonitor" )

    public:
        Win32CriticalErrorsMonitorPlugin();
        ~Win32CriticalErrorsMonitorPlugin() override;

    protected:
        bool _availablePlugin() const override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        static bool s_writeCrashDump( EXCEPTION_POINTERS * pExceptionPointers );
        static LONG WINAPI s_exceptionHandler( EXCEPTION_POINTERS * pExceptionPointers );

    protected:
        Char m_dumpPath[MENGINE_MAX_PATH] = {'\0'};
    };
}