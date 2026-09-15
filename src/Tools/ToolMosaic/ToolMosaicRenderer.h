#pragma once

#include "Mosaic/RendererAdapter.hpp"

#include <cstdint>
#include <map>

namespace Mengine
{
    class ToolMosaicRenderer
        : public Mosaic::RendererAdapter
    {
    public:
        ToolMosaicRenderer();
        ~ToolMosaicRenderer() override;

    public:
        bool initialize();
        void finalize();

    public:
        Mosaic::TextureHandle createTexture( uint32_t _width, uint32_t _height, Mosaic::ByteSpan _rgbaPixels ) override;
        Mosaic::TextureHandle createMaskTexture( uint32_t _width, uint32_t _height, Mosaic::ByteSpan _maskPixels ) override;
        bool updateTexture( Mosaic::TextureHandle _texture, uint32_t _width, uint32_t _height, Mosaic::ByteSpan _rgbaPixels ) override;
        bool updateTextureRegion( Mosaic::TextureHandle _texture, uint32_t _x, uint32_t _y, uint32_t _width, uint32_t _height, uint32_t _bytesPerRow, Mosaic::ByteSpan _rgbaPixels ) override;
        bool updateMaskTextureRegion( Mosaic::TextureHandle _texture, uint32_t _x, uint32_t _y, uint32_t _width, uint32_t _height, uint32_t _bytesPerRow, Mosaic::ByteSpan _maskPixels ) override;
        void destroyTexture( Mosaic::TextureHandle _texture ) override;
        void render( const Mosaic::FrameViewport & _viewport, const Mosaic::RenderMesh & _mesh ) override;

    public:
        uint32_t getTextureName( Mosaic::TextureHandle _texture ) const;

    protected:
        Mosaic::TextureHandle createTexture_( uint32_t _width, uint32_t _height, Mosaic::ByteSpan _pixels, bool _mask );
        bool updateTextureRegion_( Mosaic::TextureHandle _texture, uint32_t _x, uint32_t _y, uint32_t _width, uint32_t _height, uint32_t _bytesPerRow, Mosaic::ByteSpan _pixels, bool _mask );

    protected:
        uint32_t m_program;
        uint32_t m_vertexArray;
        uint32_t m_vertexBuffer;
        uint32_t m_indexBuffer;
        int32_t m_projectionLocation;
        int32_t m_textureLocation;

        typedef std::map<Mosaic::TextureHandle, uint32_t> MapTextures;
        MapTextures m_textures;

        Mosaic::TextureHandle m_handleEnumerator;
    };
}
