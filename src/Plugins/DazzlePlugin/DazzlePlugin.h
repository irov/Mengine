#pragma once

#include "DazzleIncluder.h"

#include "Kernel/PluginBase.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "DazzleScriptEmbedding.h"
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DazzlePlugin
        : public PluginBase
        , protected EXTEND_EMBEDDABLE( DazzleScriptEmbedding )
    {
        PLUGIN_DECLARE( "Dazzle" )
        DECLARE_EMBEDDABLE();

    public:
        DazzlePlugin();
        ~DazzlePlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        void _destroyPlugin() override;

    protected:
        dz_service_t * m_service;
    };
    //////////////////////////////////////////////////////////////////////////
}
