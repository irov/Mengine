#include "TTFAtlasService.h"

#include "Interface/RenderTextureServiceInterface.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/TextureHelper.h"
#include "Kernel/PixelFormatHelper.h"
#include "Kernel/NotificationHelper.h"

#include "Config/StdUtility.h"

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
        m_minAtlasPow = CONFIG_VALUE_INTEGER( "TTFPlugin", "MinAtlasPow", 4U );
        m_maxAtlasPow = CONFIG_VALUE_INTEGER( "TTFPlugin", "MaxAtlasPow", 16U );
        m_maxAtlasWidth = CONFIG_VALUE_INTEGER( "TTFPlugin", "MaxAtlasWidth", 2048U );

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_RENDER_DEVICE_LOST_PREPARE, &TTFAtlasService::notifyRenderDeviceLostPrepare, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFAtlasService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_RENDER_DEVICE_LOST_PREPARE );

        this->clearAtlasess_();
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureInterfacePtr TTFAtlasService::makeTextureGlyph( uint32_t _width, uint32_t _height, uint32_t _border, uint32_t _channel, const TTFTextureGlyphProviderInterface * _provider, mt::uv4f * const _uv, const DocumentInterfacePtr & _doc )
    {
        if( _width == 0 || _height == 0 )
        {
            return nullptr;
        }

        TTFAtlas * atlas = this->getAtlas_( _width + _border * 2, _height + _border * 2, _channel, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( atlas, "invalid get atlas" );

        uint32_t index = atlas->indices.back();
        atlas->indices.pop_back();

        const RenderTextureInterfacePtr & texture = atlas->texture;

        MENGINE_ASSERTION_MEMORY_PANIC( texture, "invalid atlas texture" );

        Rect rect;
        rect.left = index * atlas->width;
        rect.top = 0;
        rect.right = rect.left + _width + _border * 2;
        rect.bottom = rect.top + _height + _border * 2;

        const RenderImageInterfacePtr & texture_image = texture->getImage();

        MENGINE_ASSERTION_MEMORY_PANIC( texture_image, "invalid texture image" );

        RenderImageLockedInterfacePtr texture_locked = texture_image->lock( 0, rect, false );

        size_t texture_pitch;
        uint8_t * texture_memory = texture_locked->getBuffer( &texture_pitch );

        MENGINE_ASSERTION_MEMORY_PANIC( texture_memory, "invalid texture memory" );

        EPixelFormat hwPixelFormat = texture_image->getHWPixelFormat();

        uint32_t hwPixelChannels = Helper::getPixelFormatChannels( hwPixelFormat );

        bool successful = _provider->onTextureGlyphFill( texture_memory, texture_pitch, hwPixelChannels, _border );

        texture_image->unlock( texture_locked, 0, successful );

        if( successful == false )
        {
            return nullptr;
        }

        float atlas_width_inv = texture->getWidthInv();
        float atlas_height_inv = texture->getHeightInv();

        _uv->p0 = mt::vec2f( float( rect.left + _border ) * atlas_width_inv, float( rect.top + _border ) * atlas_height_inv );
        _uv->p1 = mt::vec2f( float( rect.right - _border ) * atlas_width_inv, float( rect.top + _border ) * atlas_height_inv );
        _uv->p2 = mt::vec2f( float( rect.right - _border ) * atlas_width_inv, float( rect.bottom - _border ) * atlas_height_inv );
        _uv->p3 = mt::vec2f( float( rect.left + _border ) * atlas_width_inv, float( rect.bottom - _border ) * atlas_height_inv );

        return texture;
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFAtlasService::clearAtlasess_()
    {
        for( uint32_t i = 0; i != 3; ++i )
        {
            MapAtlasess & atlas = m_atlasess[i];

            atlas.clear();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    TTFAtlasService::TTFAtlas * TTFAtlasService::getAtlas_( uint32_t _width, uint32_t _height, uint32_t _channel, const DocumentInterfacePtr & _doc )
    {
        MENGINE_ASSERTION_FATAL( _channel < 5, "invalid channel %u"
            , _channel
        );

        uint32_t hw_width = Helper::getTexturePow2( _width );
        uint32_t hw_height = Helper::getTexturePow2( _height );

        uint32_t dimension = MENGINE_MAX( hw_width, hw_height );

        uint32_t dimension_np2 = mt::get_np2( dimension );

        uint32_t atlasIndex = dimension_np2 > m_minAtlasPow ? dimension_np2 - m_minAtlasPow : 0;

        uint32_t mixAtlasPow = m_maxAtlasPow - m_minAtlasPow;

        if( atlasIndex > mixAtlasPow )
        {
            return nullptr;
        }

        const uint32_t unpow_channel_mask[] = {0, 0, 1, 0, 2};

        uint32_t unpow_channel = unpow_channel_mask[_channel];

        AtlasKey key = StdUtility::make_pair( hw_width, hw_height );

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

        EPixelFormat format = PF_A8R8G8B8;

        RenderTextureInterfacePtr texture = RENDERTEXTURE_SERVICE()
            ->createTexture( 1, m_maxAtlasWidth, newAtlas.height, format, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( texture, "invalid create texture" );

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
    //////////////////////////////////////////////////////////////////////////
    void TTFAtlasService::notifyRenderDeviceLostPrepare()
    {
        this->clearAtlasess_();
    }
    //////////////////////////////////////////////////////////////////////////
}
