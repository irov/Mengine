#pragma once

#include "MosaicInterface.h"

#include "Interface/RenderTextureInterface.h"

#include "Kernel/Map.h"

#include "Mosaic/RendererAdapter.hpp"

namespace Mengine
{
    class MosaicRendererAdapter
        : public Mosaic::RendererAdapter
    {
    public:
        MosaicRendererAdapter();
        ~MosaicRendererAdapter() override;

    public:
        Mosaic::TextureHandle createTexture( uint32_t _width, uint32_t _height, Mosaic::ByteSpan _rgbaPixels ) override;
        Mosaic::TextureHandle createMaskTexture( uint32_t _width, uint32_t _height, Mosaic::ByteSpan _maskPixels ) override;

        bool updateTexture( Mosaic::TextureHandle _texture, uint32_t _width, uint32_t _height, Mosaic::ByteSpan _rgbaPixels ) override;
        bool updateTextureRegion( Mosaic::TextureHandle _texture, uint32_t _x, uint32_t _y, uint32_t _width, uint32_t _height, uint32_t _bytesPerRow, Mosaic::ByteSpan _rgbaPixels ) override;
        bool updateMaskTextureRegion( Mosaic::TextureHandle _texture, uint32_t _x, uint32_t _y, uint32_t _width, uint32_t _height, uint32_t _bytesPerRow, Mosaic::ByteSpan _maskPixels ) override;

        void destroyTexture( Mosaic::TextureHandle _texture ) override;

        void render( const Mosaic::FrameViewport & _viewport, const Mosaic::RenderMesh & _mesh ) override;

    public:
        Mosaic::TextureHandle acquireTexture( const RenderTextureInterfacePtr & _texture );
        Mosaic::TextureHandle acquireGlyphTexture( const RenderTextureInterfacePtr & _texture, bool _premultiply );
        bool isGlyphTexture( Mosaic::TextureHandle _handle ) const;
        bool isGlyphTexturePremultiply( Mosaic::TextureHandle _handle ) const;
        const RenderTextureInterfacePtr & lookupTexture( Mosaic::TextureHandle _handle ) const;
        void releaseTexture( Mosaic::TextureHandle _handle );

    public:
        void finalize();

    protected:
        Mosaic::TextureHandle createTexture_( uint32_t _width, uint32_t _height, Mosaic::ByteSpan _pixels, bool _mask );
        bool updateTextureRegion_( Mosaic::TextureHandle _texture, uint32_t _x, uint32_t _y, uint32_t _width, uint32_t _height, uint32_t _bytesPerRow, Mosaic::ByteSpan _pixels, bool _mask );

    protected:
        typedef Map<Mosaic::TextureHandle, RenderTextureInterfacePtr> MapMosaicTextures;
        MapMosaicTextures m_textures;

        typedef Map<Mosaic::TextureHandle, bool> MapMosaicGlyphTextures;
        MapMosaicGlyphTextures m_glyphTextures;

        Mosaic::TextureHandle m_handleEnumerator;
    };
}
