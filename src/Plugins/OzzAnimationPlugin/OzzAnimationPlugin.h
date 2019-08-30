#pragma once

#include "Interface/CodecInterface.h"
#include "Interface/RenderVertexAttributeInterface.h"

#include "Kernel/PluginBase.h"
#include "Kernel/Observable.h"

#include "Config/Vector.h"

namespace Mengine
{
    class OzzAnimationPlugin
        : public PluginBase
        , public Observable
    {
        PLUGIN_DECLARE( "OzzAnimation" )

    public:
        OzzAnimationPlugin();
        ~OzzAnimationPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}