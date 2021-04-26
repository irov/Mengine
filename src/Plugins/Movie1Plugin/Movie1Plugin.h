#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class Movie1Plugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "Movie1" )

    public:
        Movie1Plugin();
        ~Movie1Plugin() override;

    protected:
        bool _availablePlugin() const override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}