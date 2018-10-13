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

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFAtlasService::_finalizeService()
    {
        for( uint32_t i = 0; i != 3; ++i )
        {
            MapAtlasess & atlas = m_atlasess[i];

            atlas.clear();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureInterfacePtr TTFAtlasService::makeTextureGlyph( uint32_t _glyphWidth, uint32_t _glyphOffset, uint32_t _glyphHeight, uint32_t _altasWidth, uint32_t _altasHeight, uint32_t _atlasChannel, TextureGlyphProviderInterface * _provider, mt::uv4f & _uv )
    {
        if( _altasWidth == 0 || _altasHeight == 0 )
        {
            return nullptr;
        }

        TTFAtlas * atlas = this->getAtlas_( _altasWidth, _altasHeight, _atlasChannel );

        if( atlas == nullptr )
        {
            return nullptr;
        }

        uint32_t index = atlas->indices.back();
        atlas->indices.pop_back();

        const RenderTextureInterfacePtr & texture = atlas->texture;

        Rect rect;
        rect.left = index * atlas->width;
        rect.top = _glyphOffset;
        rect.right = rect.left + _glyphWidth;
        rect.bottom = rect.top + _glyphHeight;

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
        _uv.p1 = mt::vec2f( float( rect.right - 1 ) * atlas_width_inv, float( rect.top + 1 ) * atlas_height_inv );
        _uv.p2 = mt::vec2f( float( rect.right - 1 ) * atlas_width_inv, float( rect.bottom - 1 ) * atlas_height_inv );
        _uv.p3 = mt::vec2f( float( rect.left + 1 ) * atlas_width_inv, float( rect.bottom - 1 ) * atlas_height_inv );

        return texture;
    }
    //////////////////////////////////////////////////////////////////////////
    TTFAtlas * TTFAtlasService::getAtlas_( uint32_t _width, uint32_t _height, uint32_t _channel )
    {
        uint32_t hw_width = Helper::getTexturePOW2( _width );
        uint32_t hw_height = Helper::getTexturePOW2( _height );

        uint32_t dimension = Helper::Max( hw_width, hw_height );

        uint32_t dimension_np2 = mt::get_np2( dimension );

        uint32_t atlasIndex = dimension_np2 > m_minAtlasPow ? dimension_np2 - m_minAtlasPow : 0;

        uint32_t mixAtlasPow = m_maxAtlasPow - m_minAtlasPow;

        if( atlasIndex > mixAtlasPow )
        {
            return nullptr;
        }

        const uint32_t unpow_channel_mask[] = { 0, 0, 1, 0, 2 };

        uint32_t unpow_channel = unpow_channel_mask[_channel];

        AtlasKey key = std::make_pair( hw_width, hw_height );

        VectorAtlases & atlases = m_atlasess[unpow_channel][key];

        for( TTFAtlas & atlas : atlases )
        {
            if( atlas.indices.empty() == true )
            {
                continue;
            }

            return &atlas;
        }

        TTFAtlas newAtlas;

        newAtlas.channel = _channel;
        newAtlas.width = hw_width;
        newAtlas.height = hw_height;

        //#   ifdef MENGINE_RENDER_TEXTURE_RGBA
        //        PixelFormat format_select[] = {PF_A8, PF_A8B8G8R8, PF_A8B8G8R8, PF_A8B8G8R8};
        //#	else
        //        PixelFormat format_select[] = {PF_A8, PF_A8R8G8B8, PF_A8R8G8B8, PF_A8R8G8B8};
        //#	endif

                //PixelFormat format = format_select[unpow_channel];

        PixelFormat format = PF_A8R8G8B8;

        RenderTextureInterfacePtr texture = RENDERTEXTURE_SERVICE()
            ->createDynamicTexture( m_maxAtlasWidth, hw_height, 4, 1, format );

        if( texture == nullptr )
        {
            return nullptr;
        }

        newAtlas.texture = texture;

        uint32_t maxGlyphCount = m_maxAtlasWidth / newAtlas.width;

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
