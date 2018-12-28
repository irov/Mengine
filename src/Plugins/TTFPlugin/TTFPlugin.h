#pragma once

#include "Interface/ThreadMutexInterface.h"

#include "Kernel/PluginBase.h"

#include "TTFServiceInterface.h"

#include "ft2build.h"
#include "freetype/freetype.h"
#include "freetype/ftglyph.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TTFPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "TTF" )

    public:
        TTFPlugin();
        ~TTFPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        void _destroy() override;

    protected:
        FT_Library m_ftlibrary;

        ThreadMutexInterfacePtr m_ftMutex;
    };
}