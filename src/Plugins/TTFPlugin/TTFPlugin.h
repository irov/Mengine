#pragma once

#include "Interface/ThreadMutexInterface.h"

#include "Kernel/PluginBase.h"

#include "TTFInterface.h"

#include "ft2build.h"
#include "freetype/freetype.h"
#include "freetype/ftglyph.h"

namespace Mengine
{    
    class TTFPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "TTF" )

    public:
        TTFPlugin();
        ~TTFPlugin() override;

    protected:
        bool _availablePlugin() const override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;

    protected:
        FT_Library m_ftlibrary;

        ThreadMutexInterfacePtr m_ftMutex;
    };
}