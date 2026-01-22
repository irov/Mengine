#pragma once

#include "Kernel/PluginBase.h"
#include "Kernel/String.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#include "Movie2ScriptEmbedding.h"
#endif

#include "movie/movie.hpp"

namespace Mengine
{
    class MoviePlugin
        : public PluginBase
        , protected EXTEND_EMBEDDABLE( Movie2ScriptEmbedding )
    {
        PLUGIN_DECLARE( "Movie" );
        DECLARE_VISITABLE( PluginInterface );
        DECLARE_EMBEDDABLE();

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