#include "MosaicRendererAdapter.h"

#include "Interface/RenderTextureServiceInterface.h"
#include "Interface/ImageCodecInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/MemoryCopy.h"
#include "Kernel/Rect.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void copyMosaicPixels_( const uint8_t * _source, size_t _sourcePitch, uint8_t * _target, size_t _targetPitch, uint32_t _width, uint32_t _height, bool _mask )
        {
            for( uint32_t y = 0; y != _height; ++y )
            {
                const uint8_t * sourceRow = _source + _sourcePitch * y;
                uint8_t * targetRow = _target + _targetPitch * y;

                for( uint32_t x = 0; x != _width; ++x )
                {
                    uint8_t * targetPixel = targetRow + x * 4;

                    if( _mask == true )
                    {
                        targetPixel[0] = 255;
                        targetPixel[1] = 255;
                        targetPixel[2] = 255;
                        targetPixel[3] = sourceRow[x];

                        continue;
                    }

                    const uint8_t * sourcePixel = sourceRow + x * 4;

#if defined(MENGINE_RENDER_TEXTURE_RGBA)
                    targetPixel[0] = sourcePixel[0];
                    targetPixel[1] = sourcePixel[1];
                    targetPixel[2] = sourcePixel[2];
#else
                    targetPixel[0] = sourcePixel[2];
                    targetPixel[1] = sourcePixel[1];
                    targetPixel[2] = sourcePixel[0];
#endif
                    targetPixel[3] = sourcePixel[3];
                }
            }
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    MosaicRendererAdapter::MosaicRendererAdapter()
        : m_handleEnumerator( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MosaicRendererAdapter::~MosaicRendererAdapter()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicRendererAdapter::finalize()
    {
        for( const MapMosaicTextures::value_type & value : m_textures )
        {
            const RenderTextureInterfacePtr & texture = value.second;

            texture->release();
        }

        m_textures.clear();
        m_glyphTextures.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    Mosaic::TextureHandle MosaicRendererAdapter::createTexture_( uint32_t _width, uint32_t _height, Mosaic::ByteSpan _pixels, bool _mask )
    {
        if( _width == 0 || _height == 0 )
        {
            return 0;
        }

        RenderTextureInterfacePtr texture = RENDERTEXTURE_SERVICE()
            ->createTexture( 1, _width, _height, PF_A8R8G8B8, DF_IMAGE_NONE, MENGINE_DOCUMENT_FACTORABLE );

        if( texture == nullptr )
        {
            LOGGER_ERROR( "invalid create mosaic texture '%ux%u'"
                , _width
                , _height
            );

            return 0;
        }

        Mosaic::TextureHandle handle = ++m_handleEnumerator;

        m_textures.emplace( handle, texture );

        if( _pixels.empty() == false )
        {
            uint32_t bytesPerRow = _width * (_mask == true ? 1 : 4);

            if( this->updateTextureRegion_( handle, 0, 0, _width, _height, bytesPerRow, _pixels, _mask ) == false )
            {
                this->destroyTexture( handle );

                return 0;
            }
        }

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    Mosaic::TextureHandle MosaicRendererAdapter::createTexture( uint32_t _width, uint32_t _height, Mosaic::ByteSpan _rgbaPixels )
    {
        Mosaic::TextureHandle handle = this->createTexture_( _width, _height, _rgbaPixels, false );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    Mosaic::TextureHandle MosaicRendererAdapter::createMaskTexture( uint32_t _width, uint32_t _height, Mosaic::ByteSpan _maskPixels )
    {
        Mosaic::TextureHandle handle = this->createTexture_( _width, _height, _maskPixels, true );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MosaicRendererAdapter::updateTexture( Mosaic::TextureHandle _texture, uint32_t _width, uint32_t _height, Mosaic::ByteSpan _rgbaPixels )
    {
        bool successful = this->updateTextureRegion_( _texture, 0, 0, _width, _height, _width * 4, _rgbaPixels, false );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MosaicRendererAdapter::updateTextureRegion( Mosaic::TextureHandle _texture, uint32_t _x, uint32_t _y, uint32_t _width, uint32_t _height, uint32_t _bytesPerRow, Mosaic::ByteSpan _rgbaPixels )
    {
        bool successful = this->updateTextureRegion_( _texture, _x, _y, _width, _height, _bytesPerRow, _rgbaPixels, false );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MosaicRendererAdapter::updateMaskTextureRegion( Mosaic::TextureHandle _texture, uint32_t _x, uint32_t _y, uint32_t _width, uint32_t _height, uint32_t _bytesPerRow, Mosaic::ByteSpan _maskPixels )
    {
        bool successful = this->updateTextureRegion_( _texture, _x, _y, _width, _height, _bytesPerRow, _maskPixels, true );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MosaicRendererAdapter::updateTextureRegion_( Mosaic::TextureHandle _texture, uint32_t _x, uint32_t _y, uint32_t _width, uint32_t _height, uint32_t _bytesPerRow, Mosaic::ByteSpan _pixels, bool _mask )
    {
        MapMosaicTextures::const_iterator it_found = m_textures.find( _texture );

        if( it_found == m_textures.end() )
        {
            return false;
        }

        if( _width == 0 || _height == 0 )
        {
            return false;
        }

        if( _pixels.size() < (size_t)_bytesPerRow * (_height - 1) + (size_t)_width * (_mask == true ? 1 : 4) )
        {
            LOGGER_ERROR( "mosaic texture update out of source range '%zu'"
                , _pixels.size()
            );

            return false;
        }

        const RenderTextureInterfacePtr & texture = it_found->second;

        const RenderImageInterfacePtr & image = texture->getImage();

        Rect rect;
        rect.left = _x;
        rect.top = _y;
        rect.right = _x + _width;
        rect.bottom = _y + _height;

        RenderImageLockedInterfacePtr locked = image->lock( 0, 0, rect, false );

        if( locked == nullptr )
        {
            LOGGER_ERROR( "invalid lock mosaic texture" );

            return false;
        }

        size_t texturePitch = 0;
        void * textureMemory = locked->getLockedBuffer( &texturePitch );

        if( textureMemory == nullptr )
        {
            image->unlock( locked, 0, 0, false );

            return false;
        }

        const uint8_t * source = reinterpret_cast<const uint8_t *>(_pixels.data());
        uint8_t * target = static_cast<uint8_t *>(textureMemory);

        Detail::copyMosaicPixels_( source, _bytesPerRow, target, texturePitch, _width, _height, _mask );

        image->unlock( locked, 0, 0, true );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicRendererAdapter::destroyTexture( Mosaic::TextureHandle _texture )
    {
        MapMosaicTextures::iterator it_found = m_textures.find( _texture );

        if( it_found == m_textures.end() )
        {
            return;
        }

        const RenderTextureInterfacePtr & texture = it_found->second;

        texture->release();

        m_textures.erase( it_found );
        m_glyphTextures.erase( _texture );
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicRendererAdapter::render( const Mosaic::FrameViewport & _viewport, const Mosaic::RenderMesh & _mesh )
    {
        MENGINE_UNUSED( _viewport );
        MENGINE_UNUSED( _mesh );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    Mosaic::TextureHandle MosaicRendererAdapter::acquireTexture( const RenderTextureInterfacePtr & _texture )
    {
        for( const MapMosaicTextures::value_type & value : m_textures )
        {
            if( value.second == _texture )
            {
                return value.first;
            }
        }

        Mosaic::TextureHandle handle = ++m_handleEnumerator;

        m_textures.emplace( handle, _texture );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    Mosaic::TextureHandle MosaicRendererAdapter::acquireGlyphTexture( const RenderTextureInterfacePtr & _texture, bool _premultiply )
    {
        Mosaic::TextureHandle handle = this->acquireTexture( _texture );

        m_glyphTextures[handle] = _premultiply;

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MosaicRendererAdapter::isGlyphTexture( Mosaic::TextureHandle _handle ) const
    {
        bool found = m_glyphTextures.find( _handle ) != m_glyphTextures.end();

        return found;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MosaicRendererAdapter::isGlyphTexturePremultiply( Mosaic::TextureHandle _handle ) const
    {
        MapMosaicGlyphTextures::const_iterator it_found = m_glyphTextures.find( _handle );

        if( it_found == m_glyphTextures.end() )
        {
            return false;
        }

        return it_found->second;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderTextureInterfacePtr & MosaicRendererAdapter::lookupTexture( Mosaic::TextureHandle _handle ) const
    {
        MapMosaicTextures::const_iterator it_found = m_textures.find( _handle );

        if( it_found == m_textures.end() )
        {
            return RenderTextureInterfacePtr::none();
        }

        const RenderTextureInterfacePtr & texture = it_found->second;

        return texture;
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicRendererAdapter::releaseTexture( Mosaic::TextureHandle _handle )
    {
        m_textures.erase( _handle );
        m_glyphTextures.erase( _handle );
    }
    //////////////////////////////////////////////////////////////////////////
}
