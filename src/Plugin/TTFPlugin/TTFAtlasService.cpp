#include "TTFAtlasService.h"

#include "Interface/RenderSystemInterface.h"

#include "Interface/ConfigInterface.h"

#include "stdex/memorycopy.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( TTFAtlasService, Mengine::TTFAtlasService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TTFAtlasService::TTFAtlasService()
        : m_minAtlasPow( 0 )
        , m_maxAtlasPow( 0 )
        , m_maxAtlasWidth( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TTFAtlasService::~TTFAtlasService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFAtlasService::_initializeService()
    {
        m_minAtlasPow = CONFIG_VALUE( "TTF", "MinAtlasPow", 4U );
        m_maxAtlasPow = CONFIG_VALUE( "TTF", "MaxAtlasPow", 16U );
        m_maxAtlasWidth = CONFIG_VALUE( "TTF", "MaxAtlasWidth", 2048U );

        uint32_t mixAtlasPow = m_maxAtlasPow - m_minAtlasPow;

        for( uint32_t i = 0; i != 3; ++i )
        {
            m_atlasess[i].resize( mixAtlasPow );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFAtlasService::_finalizeService()
    {
        for( uint32_t i = 0; i != 3; ++i )
        {
            VectorAtlasess & atlas = m_atlasess[i];

            atlas.clear();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureInterfacePtr TTFAtlasService::makeTextureGlyph( uint32_t _width, uint32_t _height, uint32_t _channel, TextureGlyphProviderInterface * _provider, mt::uv4f & _uv )
    {
        if( _width == 0 || _height == 0 )
        {
            return nullptr;
        }

        uint32_t border_width = _width + 2;
        uint32_t border_height = _height + 2;

        uint32_t hw_width = Helper::getTexturePOW2( border_width );
        uint32_t hw_height = Helper::getTexturePOW2( border_height );

        uint32_t dimension = Helper::Max( hw_width, hw_height );

        TTFAtlas * atlas = this->getAtlas_( dimension, _channel );

        if( atlas == nullptr )
        {
            return nullptr;
        }

        uint32_t index = atlas->indices.back();
        atlas->indices.pop_back();

        const RenderTextureInterfacePtr & texture = atlas->texture;

        Rect rect;
        rect.left = index * atlas->dimension;
        rect.top = 0;
        rect.right = rect.left + border_width;
        rect.bottom = border_height;

        const RenderImageInterfacePtr & texture_image = texture->getImage();

        if( texture_image == nullptr )
        {
            return nullptr;
        }

        size_t texture_pitch;
        uint8_t * texture_memory = texture_image->lock( &texture_pitch, 0, rect, false );

        uint32_t texture_channel = texture_image->getHWChannels();

        bool successful = _provider->onTextureGlyphFill( texture_memory, texture_pitch, texture_channel );

        texture_image->unlock( 0, successful );

        if( successful == false )
        {
            return nullptr;
        }

        float atlas_width_inv = texture->getWidthInv();
        float atlas_height_inv = texture->getHeightInv();

        _uv.p0 = mt::vec2f( float( rect.left + 1 ) * atlas_width_inv, float( rect.top + 1 ) * atlas_height_inv );
        _uv.p1 = mt::vec2f( float( rect.left + 1 + _width ) * atlas_width_inv, float( rect.top + 1 ) * atlas_height_inv );
        _uv.p2 = mt::vec2f( float( rect.left + 1 + _width ) * atlas_width_inv, float( rect.top + 1 + _height ) * atlas_height_inv );
        _uv.p3 = mt::vec2f( float( rect.left + 1 ) * atlas_width_inv, float( rect.top + 1 + _height ) * atlas_height_inv );

        return texture;
    }
    //////////////////////////////////////////////////////////////////////////
    TTFAtlas * TTFAtlasService::getAtlas_( uint32_t _dimension, uint32_t _channel )
    {
        uint32_t dimension_np2 = mt::get_np2( _dimension );

        uint32_t atlasIndex = dimension_np2 > m_minAtlasPow ? dimension_np2 - m_minAtlasPow : 0;

        uint32_t mixAtlasPow = m_maxAtlasPow - m_minAtlasPow;

        if( atlasIndex > mixAtlasPow )
        {
            return nullptr;
        }

        const uint32_t unpow_channel_mask[] = { 0, 0, 1, 0, 2 };

        uint32_t unpow_channel = unpow_channel_mask[_channel];

        VectorAtlases & atlases = m_atlasess[unpow_channel][atlasIndex];

        for( TTFAtlas & atlas : atlases )
        {
            if( atlas.indices.empty() == true )
            {
                continue;
            }

            return &atlas;
        }

        uint32_t minDimension = Helper::Power2( m_minAtlasPow );
        uint32_t fixDimension = Helper::Max( minDimension, _dimension );

        TTFAtlas newAtlas;

        newAtlas.channel = _channel;
        newAtlas.dimension = fixDimension;

        //#   ifdef MENGINE_RENDER_TEXTURE_RGBA
        //        PixelFormat format_select[] = {PF_A8, PF_A8B8G8R8, PF_A8B8G8R8, PF_A8B8G8R8};
        //#	else
        //        PixelFormat format_select[] = {PF_A8, PF_A8R8G8B8, PF_A8R8G8B8, PF_A8R8G8B8};
        //#	endif

                //PixelFormat format = format_select[unpow_channel];

        PixelFormat format = PF_A8R8G8B8;

        RenderTextureInterfacePtr texture = RENDERTEXTURE_SERVICE()
            ->createDynamicTexture( m_maxAtlasWidth, fixDimension, 4, 1, format );

        newAtlas.texture = texture;

        uint32_t maxGlyphCount = m_maxAtlasWidth / fixDimension;

        newAtlas.indices.reserve( maxGlyphCount );

        for( uint32_t i = maxGlyphCount; i != 0; --i )
        {
            newAtlas.indices.emplace_back( i - 1 );
        }

        atlases.emplace_back( newAtlas );

        TTFAtlas & atlas = atlases.back();

        return &atlas;
    }
}
