#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/UnknownInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/ContentInterface.h"
#include "Interface/FontGlyphInterface.h"
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
    class UnknownTTFFontGlyphInterface
        : public UnknownInterface
    {
    public:
        virtual void setGlyphContent( const ContentInterfacePtr & _glyphContent ) = 0;
        virtual const ContentInterfacePtr & getGlyphContent() const = 0;

    public:
        virtual void setLicenseContent( const ContentInterfacePtr & _licenseContent ) = 0;
        virtual const ContentInterfacePtr & getLicenseContent() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    class TTFTextureGlyphProviderInterface
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