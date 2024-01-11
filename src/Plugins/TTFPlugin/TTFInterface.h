#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/UnknownInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/RenderTextureInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/FilePath.h"

#include "math/uv4.h"

#ifndef MENGINE_TTF_METRICS_SYMBOL
#define MENGINE_TTF_METRICS_SYMBOL 'A'
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class UnknownTTFFontInterface
        : public UnknownInterface
    {
    };
    //////////////////////////////////////////////////////////////////////////
    class TTFTextureGlyphProviderInterface
        : public Mixin
    {
    public:
        virtual bool onTextureGlyphFill( uint8_t * const _memory, size_t _pitch, uint32_t _channel, uint32_t _border ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    class TTFAtlasServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "TTFAtlasService" )

    public:
        virtual RenderTextureInterfacePtr makeTextureGlyph( uint32_t _width, uint32_t _height, uint32_t _border, uint32_t _channel, const TTFTextureGlyphProviderInterface * _provider, mt::uv4f * const _uv, const DocumentInterfacePtr & _doc ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define TTFATLAS_SERVICE()\
    ((Mengine::TTFAtlasServiceInterface *)SERVICE_GET(Mengine::TTFAtlasServiceInterface))
//////////////////////////////////////////////////////////////////////////