#pragma once

#include "Kernel/PluginBase.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "OzzScriptEmbedding.h"
#endif

#include "ozz/base/memory/allocator.h"

namespace Mengine
{
    class OzzAnimationPlugin
        : public PluginBase
        , EXTEND_EMBEDDABLE( OzzScriptEmbedding )
    {
        PLUGIN_DECLARE( "OzzAnimation" );
        DECLARE_VISITABLE( PluginInterface );
        DECLARE_EMBEDDABLE();

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