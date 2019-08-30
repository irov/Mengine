#pragma once

#include "Kernel/PluginBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Movie1Plugin
        : public PluginBase
        , public Observable
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
    };
}