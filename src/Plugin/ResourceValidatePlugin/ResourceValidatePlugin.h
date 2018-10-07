#pragma once

#include "Kernel/PluginBase.h"
#include "Kernel/Visitor.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceValidatePlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "ResourceValidate" )

    public:
        ResourceValidatePlugin();
        ~ResourceValidatePlugin() override;

    protected:
        bool _avaliable() override;

    protected:
        bool _initialize() override;
        void _finalize() override;

    protected:
        void _destroy() override;

    protected:
        VisitorPtr m_resourceValidateVisitor;
    };
}