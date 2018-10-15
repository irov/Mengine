#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/Servant.h"
#include "Config/String.h"

#include "math/uv4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class RenderTextureInterface> RenderTextureInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class TextureGlyphProviderInterface
    {
    public:
        virtual bool onTextureGlyphFill( uint8_t * _memory, size_t _pitch, uint32_t _channel ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    class TTFAtlasServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "TTFAtlasService" )

    public:
        virtual RenderTextureInterfacePtr makeTextureGlyph( uint32_t _width, uint32_t _height, uint32_t _border, uint32_t _channel, TextureGlyphProviderInterface * _provider, mt::uv4f & _uv ) = 0;
    };
}

#define TTFATLAS_SERVICE()\
    ((Mengine::TTFAtlasServiceInterface *)SERVICE_GET(Mengine::TTFAtlasServiceInterface))

#define TTFFONT_SERVICE()\
    ((Mengine::TTFFontManagerInterface *)SERVICE_GET(Mengine::TTFFontManagerInterface))
