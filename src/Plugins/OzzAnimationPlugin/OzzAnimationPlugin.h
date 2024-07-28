#pragma once

#include "Kernel/PluginBase.h"

#include "ozz/base/memory/allocator.h"

namespace Mengine
{
    class OzzAnimationPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "OzzAnimation" )

    public:
        OzzAnimationPlugin();
        ~OzzAnimationPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        ozz::memory::Allocator * m_allocatorOld;
    };
}