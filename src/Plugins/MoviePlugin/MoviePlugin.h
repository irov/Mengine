#pragma once

#include "Kernel/PluginBase.h"

#include "movie/movie.hpp"

namespace Mengine
{
    class MoviePlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "Movie" )

    public:
        MoviePlugin();
        ~MoviePlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        const aeMovieInstance * m_movieInstance;

        String m_hashkey;
    };
}