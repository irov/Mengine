#pragma once

#include "Interface/DecoderFactoryInterface.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class TheoraPlugin
        : public PluginBase
    {
    public:
        PLUGIN_DECLARE( "Theora" )

    public:
        TheoraPlugin();
        ~TheoraPlugin() override;

    protected:
        bool _initialize() override;
        void _finalize() override;

    protected:
        typedef Vector<DecoderFactoryInterfacePtr> VectorDecoderFactories;
        VectorDecoderFactories m_decoderFactories;
    };
}

