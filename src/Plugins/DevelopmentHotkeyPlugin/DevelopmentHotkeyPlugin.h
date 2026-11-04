#pragma once

#include "Kernel/PluginBase.h"
#include "Kernel/GlobalHotkeys.h"
#include "Kernel/FilePath.h"
#include "Kernel/Resource.h"

namespace Mengine
{
    class DevelopmentHotkeyPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "DevelopmentHotkey" );

    public:
        DevelopmentHotkeyPlugin();
        ~DevelopmentHotkeyPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        bool _startPlugin() override;
        void _stopPlugin() override;

    protected:
        void addTimeFactorHotkey_( EKeyCode _code, float _timeFactorBase );
        void addTimeFactorCountHotkey_( EKeyCode _code, int32_t _timeFactorStep );

    protected:
        void updateDebugOpenFile_();
        void notifyDebugOpenFile_( const FilePath & _folder, const FilePath & _filePath, bool _read, bool _streaming );

    protected:
        void updateDebugResourceCompile_();
        void notifyDebugResourceCompile_( Resource * _resource );
        void notifyDebugResourceRelease_( Resource * _resource );

    protected:
        void reloadLocale_();

    protected:
        GlobalHotkeys m_hotkeys;

        bool m_debugFileOpen;
        bool m_debugResourceCompile;
        bool m_debugTextMode;
    };
}
