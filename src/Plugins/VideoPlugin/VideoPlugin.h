#pragma once

#include "Interface/CodecInterface.h"

#include "Kernel/PluginBase.h"
#include "Kernel/Visitor.h"

namespace Mengine
{
    class VideoPlugin
        : public PluginBase
    {
    public:
        PLUGIN_DECLARE( "Video" )

    public:
		VideoPlugin();

    protected:
        bool _initialize() override;
        void _finalize() override;

    protected:
		VisitorPtr m_videoValidateVisitor;
    };
}

