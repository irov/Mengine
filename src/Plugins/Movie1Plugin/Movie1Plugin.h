#pragma once

#include "Kernel/PluginBase.h"
#include "Kernel/Visitor.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Movie1Plugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "Movie1" )

    public:
        Movie1Plugin();
        ~Movie1Plugin() override;

    protected:
        bool _avaliable() override;

    protected:
        bool _initialize() override;
        void _finalize() override;

    protected:
        VisitorPtr m_resourceValidateVisitor;
    };
}