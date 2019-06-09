#pragma once

#include "TTFServiceInterface.h"

#include "Kernel/ServiceBase.h"

#include "Config/Vector.h"
#include "Config/Pair.h"
#include "Config/Map.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct TTFAtlas
    {
        uint32_t width;
        uint32_t height;
        uint32_t channel;

        typedef Vector<uint32_t> VectorFreeIndices;
        VectorFreeIndices indices;

        RenderTextureInterfacePtr texture;
    };
    //////////////////////////////////////////////////////////////////////////
    class TTFAtlasService
        : public ServiceBase<TTFAtlasServiceInterface>
    {
    public:
        TTFAtlasService();
        ~TTFAtlasService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        RenderTextureInterfacePtr makeTextureGlyph( uint32_t _width, uint32_t _height, uint32_t _border, uint32_t _channel, TextureGlyphProviderInterface * _provider, mt::uv4f & _uv ) override;

    protected:
        TTFAtlas * getAtlas_( uint32_t _width, uint32_t _height, uint32_t _channel );

    public:
        typedef Vector<TTFAtlas> VectorAtlases;
        typedef Pair<uint32_t, uint32_t> AtlasKey;
        typedef Map<AtlasKey, VectorAtlases> MapAtlasess;
        MapAtlasess m_atlasess[3];

        uint32_t m_minAtlasPow;
        uint32_t m_maxAtlasPow;
        uint32_t m_maxAtlasWidth;
    };
}