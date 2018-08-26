#pragma once

#include "Interface/CodecInterface.h"
#include "Interface/DataInterface.h"

#include "Kernel/PluginBase.h"

#include "Config/Vector.h"

namespace Mengine
{
    class ImageCodecPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "ImageCodec" )

    public:
        ImageCodecPlugin();

    protected:
        bool _initialize() override;
        void _finalize() override;

    protected:
        DataflowFactoryInterfacePtr m_factoryAEK;
    };
}