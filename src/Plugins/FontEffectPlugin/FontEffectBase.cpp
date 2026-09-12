#include "FontEffectBase.h"

#include "Kernel/Logger.h"
#include "Kernel/ProfilerHelper.h"

#include "Config/StdMath.h"

#include "math/constant.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        enum EFontEffectScratchPlane
        {
            EFESP_ALPHA = 0,
            EFESP_SDF,
            EFESP_TMP0,
            EFESP_TMP1,
            EFESP_WORK0,
            EFESP_WORK1,
            EFESP_LINE_UNUSED0,
            EFESP_LINE_UNUSED1
        };
        //////////////////////////////////////////////////////////////////////////
        enum EFontEffectScratchImage
        {
            EFESI_SOURCE = 0,
            EFESI_ACCUMULATOR,
            EFESI_EFFECT
        };
        //////////////////////////////////////////////////////////////////////////
        static int32_t roundToInt( float _value )
        {
            return (int32_t)StdMath::floorf( _value + 0.5f );
        }
        //////////////////////////////////////////////////////////////////////////
        static float getEffectExtent( const FontEffectStyleDesc & _effect )
        {
            switch( _effect.type )
            {
            case EFET_OUTLINE:
                return _effect.width + _effect.sharpness + 1.f;
            case EFET_SHADOW:
                {
                    float offset_x = StdMath::fabsf( _effect.offset.x );
                    float offset_y = StdMath::fabsf( _effect.offset.y );

                    float extent = offset_x + offset_y + _effect.spread + 3.f * _effect.blur + 1.f;

                    return extent;
                }
            case EFET_GLOW:
                return _effect.spread + 3.f * _effect.blur + 1.f;
            case EFET_BEVEL:
                return _effect.size + _effect.soften + 1.f;
            case EFET_BLUR:
                return 3.f * _effect.blur + 1.f;
            default:
                break;
            }

            return 0.f;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool setPositive( float * const _field, float _value, const Char * _name )
        {
            if( _value < 0.f )
            {
                LOGGER_ERROR( "font effect invalid %s %f"
                    , _name
                    , _value
                );

                return false;
            }

            *_field = _value;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    FontEffectBase::FontEffectBase()
        : m_effectSample( 1 )
        , m_effectSampleInv( 1.f )
        , m_effectSampleExplicit( false )
        , m_layoutCount( 1 )
        , m_padding( 0 )
        , m_defaultFill( true )
        , m_sdfValid( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FontEffectBase::~FontEffectBase()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectBase::setContent( const ContentInterfacePtr & _content )
    {
        m_content = _content;
    }
    //////////////////////////////////////////////////////////////////////////
    const ContentInterfacePtr & FontEffectBase::getContent() const
    {
        return m_content;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectBase::setEffectSample( uint32_t _effectSample )
    {
        m_effectSample = (_effectSample == 0) ? 1 : _effectSample;
        m_effectSampleInv = 1.f / (float)m_effectSample;
        m_effectSampleExplicit = true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t FontEffectBase::getEffectSample() const
    {
        return m_effectSample;
    }
    //////////////////////////////////////////////////////////////////////////
    float FontEffectBase::getEffectSampleInv() const
    {
        return m_effectSampleInv;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t FontEffectBase::getLayoutCount() const
    {
        return m_layoutCount;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectBase::applySampleHint_( uint32_t _sample )
    {
        if( m_effectSampleExplicit == true )
        {
            return;
        }

        if( _sample == 0 )
        {
            return;
        }

        m_effectSample = _sample;
        m_effectSampleInv = 1.f / (float)m_effectSample;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::_compile()
    {
        if( m_desc.layers.empty() == true )
        {
            LOGGER_ERROR( "font effect invalid desc: no layers" );

            return false;
        }

        for( const FontEffectLayerDesc & layer : m_desc.layers )
        {
            for( const FontEffectStyleDesc & effect : layer.styles )
            {
                if( effect.type != EFET_FILL )
                {
                    continue;
                }

                if( effect.gradient.enabled == true && effect.gradient.stops.empty() == true )
                {
                    LOGGER_ERROR( "font effect invalid desc: gradient without stops" );

                    return false;
                }
            }
        }

        uint32_t layoutCount = 0;

        for( const FontEffectLayerDesc & layer : m_desc.layers )
        {
            if( layer.enabled == false )
            {
                continue;
            }

            ++layoutCount;
        }

        if( layoutCount == 0 )
        {
            m_defaultFill = true;
            m_layoutCount = 1;
        }
        else
        {
            m_defaultFill = false;
            m_layoutCount = layoutCount;
        }

        float extent = 0.f;

        for( const FontEffectLayerDesc & layer : m_desc.layers )
        {
            if( layer.enabled == false )
            {
                continue;
            }

            for( const FontEffectStyleDesc & effect : layer.styles )
            {
                if( effect.enabled == false )
                {
                    continue;
                }

                float effect_extent = Detail::getEffectExtent( effect );

                if( effect_extent > extent )
                {
                    extent = effect_extent;
                }
            }
        }

        m_padding = (uint32_t)StdMath::ceilf( extent * (float)m_effectSample );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::setupDesc( const FontEffectDesc & _desc )
    {
        for( const FontEffectLayerDesc & layer : _desc.layers )
        {
            uint32_t layerIndex;
            if( this->addLayer( &layerIndex ) == false )
            {
                return false;
            }

            if( this->setLayerEnabled( layerIndex, layer.enabled ) == false )
            {
                return false;
            }

            if( this->setLayerOpacity( layerIndex, layer.opacity ) == false )
            {
                return false;
            }

            for( const FontEffectStyleDesc & effect : layer.styles )
            {
                uint32_t effectIndex;
                if( this->addEffect( layerIndex, effect.type, &effectIndex ) == false )
                {
                    return false;
                }

                if( this->setEffectEnabled( layerIndex, effectIndex, effect.enabled ) == false )
                {
                    return false;
                }

                if( this->setEffectOpacity( layerIndex, effectIndex, effect.opacity ) == false )
                {
                    return false;
                }

                if( this->setEffectColor( layerIndex, effectIndex, effect.color ) == false )
                {
                    return false;
                }

                if( this->setEffectWidth( layerIndex, effectIndex, effect.width ) == false )
                {
                    return false;
                }

                if( this->setEffectSharpness( layerIndex, effectIndex, effect.sharpness ) == false )
                {
                    return false;
                }

                if( this->setEffectOffset( layerIndex, effectIndex, effect.offset ) == false )
                {
                    return false;
                }

                if( this->setEffectBlur( layerIndex, effectIndex, effect.blur ) == false )
                {
                    return false;
                }

                if( this->setEffectSpread( layerIndex, effectIndex, effect.spread ) == false )
                {
                    return false;
                }

                if( this->setEffectDepth( layerIndex, effectIndex, effect.depth ) == false )
                {
                    return false;
                }

                if( this->setEffectSize( layerIndex, effectIndex, effect.size ) == false )
                {
                    return false;
                }

                if( this->setEffectSoften( layerIndex, effectIndex, effect.soften ) == false )
                {
                    return false;
                }

                if( this->setEffectAngle( layerIndex, effectIndex, effect.angle ) == false )
                {
                    return false;
                }

                if( this->setEffectAltitude( layerIndex, effectIndex, effect.altitude ) == false )
                {
                    return false;
                }

                if( this->setEffectHighlight( layerIndex, effectIndex, effect.highlight ) == false )
                {
                    return false;
                }

                if( this->setEffectShadow( layerIndex, effectIndex, effect.shadow ) == false )
                {
                    return false;
                }

                if( this->setEffectGradientEnabled( layerIndex, effectIndex, effect.gradient.enabled ) == false )
                {
                    return false;
                }

                if( this->setEffectGradientAngle( layerIndex, effectIndex, effect.gradient.angle ) == false )
                {
                    return false;
                }

                if( this->setEffectGradientSpace( layerIndex, effectIndex, effect.gradient.space ) == false )
                {
                    return false;
                }

                for( const FontEffectGradientStop & stop : effect.gradient.stops )
                {
                    if( this->addEffectGradientStop( layerIndex, effectIndex, stop.t, stop.color ) == false )
                    {
                        return false;
                    }
                }
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectBase::_release()
    {
        m_desc.layers.clear();

        m_scratch.clear();

        VectorFontEffectExportBuffer exportBuffer;
        m_exportBuffer.swap( exportBuffer );
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectBase::ensureSignedDistance_()
    {
        if( m_sdfValid == true )
        {
            return;
        }

        FontEffectPlane alpha = m_scratch.getPlane( Detail::EFESP_ALPHA );
        FontEffectPlane sdf = m_scratch.getPlane( Detail::EFESP_SDF );
        FontEffectPlane tmp0 = m_scratch.getPlane( Detail::EFESP_TMP0 );
        FontEffectPlane tmp1 = m_scratch.getPlane( Detail::EFESP_TMP1 );

        Helper::fontEffectSignedDistance( alpha, sdf, tmp0, tmp1, m_scratch.getLine( 0 ), m_scratch.getLine( 1 ), m_scratch.getLine( 2 ), m_scratch.getLine( 3 ) );

        m_sdfValid = true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::applyLayer_( const FontEffectLayerDesc & _layer, uint32_t _layoutIndex, uint32_t _width, uint32_t _rows, uint32_t _channel, int32_t _left, int32_t _top, uint32_t _height, const LambdaFontEffectProvider & _provider )
    {
        float sample = (float)m_effectSample;
        uint32_t padding = m_padding;

        FontEffectPlane alpha = m_scratch.getPlane( Detail::EFESP_ALPHA );
        FontEffectPlane tmp0 = m_scratch.getPlane( Detail::EFESP_TMP0 );
        FontEffectPlane tmp1 = m_scratch.getPlane( Detail::EFESP_TMP1 );
        FontEffectPlane work0 = m_scratch.getPlane( Detail::EFESP_WORK0 );
        FontEffectPlane work1 = m_scratch.getPlane( Detail::EFESP_WORK1 );

        FontEffectImage source = m_scratch.getImage( Detail::EFESI_SOURCE );
        FontEffectImage accumulator = m_scratch.getImage( Detail::EFESI_ACCUMULATOR );
        FontEffectImage effectImage = m_scratch.getImage( Detail::EFESI_EFFECT );

        Helper::fontEffectImageClear( accumulator );

        for( const FontEffectStyleDesc & effect : _layer.styles )
        {
            if( effect.enabled == false )
            {
                continue;
            }

            switch( effect.type )
            {
            case EFET_FILL:
                {
                    if( _channel == 4 )
                    {
                        Helper::fontEffectTintImage( source, effect.color, effect.opacity, effectImage );
                    }
                    else if( effect.gradient.enabled == true && effect.gradient.stops.empty() == false )
                    {
                        FontEffectGradientContext context;

                        float angle_rad = effect.gradient.angle * mt::constant::deg2rad;

                        context.dirX = StdMath::cosf( angle_rad );
                        context.dirY = StdMath::sinf( angle_rad );

                        float glyph_w = (float)_width;
                        float glyph_h = (float)_rows;

                        if( effect.gradient.space == EFEGS_FONT )
                        {
                            float font_h = (float)_height * sample;

                            context.centerX = (float)padding + glyph_w * 0.5f;
                            context.centerY = (float)padding + (float)_top - font_h * 0.5f;

                            float extent = StdMath::fabsf( glyph_w * context.dirX ) + StdMath::fabsf( font_h * context.dirY );

                            context.extentInv = (extent > 0.f) ? 1.f / extent : 1.f;
                        }
                        else
                        {
                            context.centerX = (float)padding + glyph_w * 0.5f;
                            context.centerY = (float)padding + glyph_h * 0.5f;

                            float extent = StdMath::fabsf( glyph_w * context.dirX ) + StdMath::fabsf( glyph_h * context.dirY );

                            context.extentInv = (extent > 0.f) ? 1.f / extent : 1.f;
                        }

                        Helper::fontEffectColorizeGradient( alpha, effect.gradient, context, effect.opacity, effectImage );
                    }
                    else
                    {
                        Helper::fontEffectColorize( alpha, effect.color, effect.opacity, effectImage );
                    }

                    Helper::fontEffectCompositeOver( accumulator, effectImage );
                }break;
            case EFET_OUTLINE:
                {
                    this->ensureSignedDistance_();

                    FontEffectPlane sdf = m_scratch.getPlane( Detail::EFESP_SDF );

                    Helper::fontEffectCoverage( sdf, effect.width * sample, effect.sharpness * sample, work0 );
                    Helper::fontEffectColorize( work0, effect.color, effect.opacity, effectImage );
                    Helper::fontEffectCompositeOver( accumulator, effectImage );
                }break;
            case EFET_SHADOW:
            case EFET_GLOW:
                {
                    if( effect.spread > 0.f )
                    {
                        this->ensureSignedDistance_();

                        FontEffectPlane sdf = m_scratch.getPlane( Detail::EFESP_SDF );

                        Helper::fontEffectCoverage( sdf, effect.spread * sample, 0.f, work0 );
                    }
                    else
                    {
                        Helper::fontEffectPlaneCopy( alpha, work0 );
                    }

                    if( effect.type == EFET_SHADOW )
                    {
                        int32_t dx = Detail::roundToInt( effect.offset.x * sample );
                        int32_t dy = Detail::roundToInt( effect.offset.y * sample );

                        if( dx != 0 || dy != 0 )
                        {
                            Helper::fontEffectOffset( work0, dx, dy, work1 );
                            Helper::fontEffectPlaneCopy( work1, work0 );
                        }
                    }

                    Helper::fontEffectBlurPlane( work0, effect.blur * sample, tmp0 );
                    Helper::fontEffectColorize( work0, effect.color, effect.opacity, effectImage );
                    Helper::fontEffectCompositeOver( accumulator, effectImage );
                }break;
            case EFET_INNER_SHADOW:
            case EFET_INNER_GLOW:
                {
                    Helper::fontEffectInvert( alpha, work0 );

                    if( effect.type == EFET_INNER_SHADOW )
                    {
                        int32_t dx = Detail::roundToInt( effect.offset.x * sample );
                        int32_t dy = Detail::roundToInt( effect.offset.y * sample );

                        if( dx != 0 || dy != 0 )
                        {
                            Helper::fontEffectOffset( work0, dx, dy, work1 );
                            Helper::fontEffectPlaneCopy( work1, work0 );
                        }
                    }

                    Helper::fontEffectBlurPlane( work0, effect.blur * sample, tmp0 );
                    Helper::fontEffectMultiply( work0, alpha );
                    Helper::fontEffectColorize( work0, effect.color, effect.opacity, effectImage );
                    Helper::fontEffectCompositeOver( accumulator, effectImage );
                }break;
            case EFET_BEVEL:
                {
                    this->ensureSignedDistance_();

                    FontEffectPlane sdf = m_scratch.getPlane( Detail::EFESP_SDF );

                    Helper::fontEffectBevel( sdf, alpha, effect, sample, work0, tmp0, effectImage );
                    Helper::fontEffectCompositeOver( accumulator, effectImage );
                }break;
            case EFET_BLUR:
                {
                    Helper::fontEffectBlurImage( accumulator, effect.blur * sample, tmp0, tmp1 );
                }break;
            default:
                break;
            }
        }

        Helper::fontEffectImageOpacity( accumulator, _layer.opacity );

        FontEffectRect rect;
        if( Helper::fontEffectTightCrop( accumulator, &rect ) == false )
        {
            rect.x = 0;
            rect.y = 0;
            rect.width = 1;
            rect.height = 1;

            Helper::fontEffectImageClear( accumulator );
        }

        size_t export_size = (size_t)rect.width * (size_t)rect.height * 4;

        if( m_exportBuffer.size() < export_size )
        {
            m_exportBuffer.resize( export_size );
        }

#if defined(MENGINE_RENDER_TEXTURE_RGBA)
        bool rgba = true;
#else
        bool rgba = false;
#endif

        Helper::fontEffectExport8( accumulator, rect, rgba, m_exportBuffer.data() );

        int32_t x = _left - (int32_t)padding + (int32_t)rect.x;
        int32_t y = -(_top + (int32_t)padding - (int32_t)rect.y);

        _provider( _layoutIndex, x, y, rect.width, rect.height, m_exportBuffer.data(), rect.width * 4, 4 );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::apply( uint32_t _width, uint32_t _rows, uint32_t _pitch, const void * _buffer, uint32_t _channel, int32_t _left, int32_t _top, uint32_t _height, const LambdaFontEffectProvider & _provider )
    {
        MENGINE_PROFILER_CATEGORY();

        if( _channel != 1 && _channel != 4 )
        {
            LOGGER_ERROR( "font effect unsupported glyph channel %u"
                , _channel
            );

            return false;
        }

        uint32_t padding = m_padding;

        uint32_t canvas_w = _width + padding * 2;
        uint32_t canvas_h = _rows + padding * 2;

        m_scratch.setup( canvas_w, canvas_h );

        m_sdfValid = false;

        FontEffectPlane alpha = m_scratch.getPlane( Detail::EFESP_ALPHA );
        FontEffectImage source = m_scratch.getImage( Detail::EFESI_SOURCE );

        if( _channel == 1 )
        {
            Helper::fontEffectImportA8( _buffer, _width, _rows, _pitch, padding, alpha );
        }
        else
        {
            Helper::fontEffectImportBGRA( _buffer, _width, _rows, _pitch, padding, alpha, source );
        }

        if( m_defaultFill == true )
        {
            FontEffectLayerDesc layer;
            layer.enabled = true;
            layer.opacity = 1.f;

            FontEffectStyleDesc fill;
            fill.type = EFET_FILL;

            layer.styles.emplace_back( fill );

            if( this->applyLayer_( layer, 0, _width, _rows, _channel, _left, _top, _height, _provider ) == false )
            {
                return false;
            }

            return true;
        }

        uint32_t layoutIndex = 0;

        for( const FontEffectLayerDesc & layer : m_desc.layers )
        {
            if( layer.enabled == false )
            {
                continue;
            }

            if( this->applyLayer_( layer, layoutIndex, _width, _rows, _channel, _left, _top, _height, _provider ) == false )
            {
                return false;
            }

            ++layoutIndex;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::prefetch( const PrefetcherObserverInterfacePtr & _observer )
    {
        bool successful = this->_prefetch( _observer );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::unfetch()
    {
        bool successful = this->_unfetch();

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::_prefetch( const PrefetcherObserverInterfacePtr & _observer )
    {
        _observer->onPrefetchPreparation();
        _observer->onPrefetchComplete( true );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::_unfetch()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::isValid() const
    {
        return m_desc.layers.empty() == false;
    }
    //////////////////////////////////////////////////////////////////////////
    FontEffectLayerDesc * FontEffectBase::getLayer_( uint32_t _layerIndex )
    {
        if( _layerIndex >= m_desc.layers.size() )
        {
            LOGGER_ERROR( "font effect invalid layer index %u"
                , _layerIndex
            );

            return nullptr;
        }

        return &m_desc.layers[_layerIndex];
    }
    //////////////////////////////////////////////////////////////////////////
    FontEffectStyleDesc * FontEffectBase::getEffect_( uint32_t _layerIndex, uint32_t _effectIndex )
    {
        FontEffectLayerDesc * layer = this->getLayer_( _layerIndex );

        if( layer == nullptr )
        {
            return nullptr;
        }

        if( _effectIndex >= layer->styles.size() )
        {
            LOGGER_ERROR( "font effect layer %u invalid effect index %u"
                , _layerIndex
                , _effectIndex
            );

            return nullptr;
        }

        return &layer->styles[_effectIndex];
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::addLayer( uint32_t * const _layerIndex )
    {
        if( m_desc.layers.size() >= MENGINE_FONTEFFECT_MAX_LAYERS )
        {
            LOGGER_ERROR( "font effect max layers %u"
                , MENGINE_FONTEFFECT_MAX_LAYERS
            );

            return false;
        }

        m_desc.layers.emplace_back();

        *_layerIndex = (uint32_t)m_desc.layers.size() - 1;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::setLayerEnabled( uint32_t _layerIndex, bool _enabled )
    {
        FontEffectLayerDesc * layer = this->getLayer_( _layerIndex );

        if( layer == nullptr )
        {
            return false;
        }

        layer->enabled = _enabled;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::setLayerOpacity( uint32_t _layerIndex, float _opacity )
    {
        FontEffectLayerDesc * layer = this->getLayer_( _layerIndex );

        if( layer == nullptr )
        {
            return false;
        }

        bool successful = Detail::setPositive( &layer->opacity, _opacity, "layer opacity" );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::addEffect( uint32_t _layerIndex, EFontEffectType _type, uint32_t * const _effectIndex )
    {
        FontEffectLayerDesc * layer = this->getLayer_( _layerIndex );

        if( layer == nullptr )
        {
            return false;
        }

        if( _type >= __EFET_MAX__ )
        {
            LOGGER_ERROR( "font effect invalid type %u"
                , _type
            );

            return false;
        }

        FontEffectStyleDesc effect;
        effect.type = _type;

        layer->styles.emplace_back( effect );

        *_effectIndex = (uint32_t)layer->styles.size() - 1;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::setEffectEnabled( uint32_t _layerIndex, uint32_t _effectIndex, bool _enabled )
    {
        FontEffectStyleDesc * effect = this->getEffect_( _layerIndex, _effectIndex );

        if( effect == nullptr )
        {
            return false;
        }

        effect->enabled = _enabled;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::setEffectOpacity( uint32_t _layerIndex, uint32_t _effectIndex, float _opacity )
    {
        FontEffectStyleDesc * effect = this->getEffect_( _layerIndex, _effectIndex );

        if( effect == nullptr )
        {
            return false;
        }

        bool successful = Detail::setPositive( &effect->opacity, _opacity, "effect opacity" );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::setEffectColor( uint32_t _layerIndex, uint32_t _effectIndex, const Color & _color )
    {
        FontEffectStyleDesc * effect = this->getEffect_( _layerIndex, _effectIndex );

        if( effect == nullptr )
        {
            return false;
        }

        effect->color = _color;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::setEffectWidth( uint32_t _layerIndex, uint32_t _effectIndex, float _width )
    {
        FontEffectStyleDesc * effect = this->getEffect_( _layerIndex, _effectIndex );

        if( effect == nullptr )
        {
            return false;
        }

        bool successful = Detail::setPositive( &effect->width, _width, "effect width" );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::setEffectSharpness( uint32_t _layerIndex, uint32_t _effectIndex, float _sharpness )
    {
        FontEffectStyleDesc * effect = this->getEffect_( _layerIndex, _effectIndex );

        if( effect == nullptr )
        {
            return false;
        }

        bool successful = Detail::setPositive( &effect->sharpness, _sharpness, "effect sharpness" );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::setEffectOffset( uint32_t _layerIndex, uint32_t _effectIndex, const mt::vec2f & _offset )
    {
        FontEffectStyleDesc * effect = this->getEffect_( _layerIndex, _effectIndex );

        if( effect == nullptr )
        {
            return false;
        }

        effect->offset = _offset;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::setEffectBlur( uint32_t _layerIndex, uint32_t _effectIndex, float _blur )
    {
        FontEffectStyleDesc * effect = this->getEffect_( _layerIndex, _effectIndex );

        if( effect == nullptr )
        {
            return false;
        }

        bool successful = Detail::setPositive( &effect->blur, _blur, "effect blur" );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::setEffectSpread( uint32_t _layerIndex, uint32_t _effectIndex, float _spread )
    {
        FontEffectStyleDesc * effect = this->getEffect_( _layerIndex, _effectIndex );

        if( effect == nullptr )
        {
            return false;
        }

        bool successful = Detail::setPositive( &effect->spread, _spread, "effect spread" );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::setEffectDepth( uint32_t _layerIndex, uint32_t _effectIndex, float _depth )
    {
        FontEffectStyleDesc * effect = this->getEffect_( _layerIndex, _effectIndex );

        if( effect == nullptr )
        {
            return false;
        }

        effect->depth = _depth;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::setEffectSize( uint32_t _layerIndex, uint32_t _effectIndex, float _size )
    {
        FontEffectStyleDesc * effect = this->getEffect_( _layerIndex, _effectIndex );

        if( effect == nullptr )
        {
            return false;
        }

        bool successful = Detail::setPositive( &effect->size, _size, "effect size" );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::setEffectSoften( uint32_t _layerIndex, uint32_t _effectIndex, float _soften )
    {
        FontEffectStyleDesc * effect = this->getEffect_( _layerIndex, _effectIndex );

        if( effect == nullptr )
        {
            return false;
        }

        bool successful = Detail::setPositive( &effect->soften, _soften, "effect soften" );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::setEffectAngle( uint32_t _layerIndex, uint32_t _effectIndex, float _angle )
    {
        FontEffectStyleDesc * effect = this->getEffect_( _layerIndex, _effectIndex );

        if( effect == nullptr )
        {
            return false;
        }

        effect->angle = _angle;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::setEffectAltitude( uint32_t _layerIndex, uint32_t _effectIndex, float _altitude )
    {
        FontEffectStyleDesc * effect = this->getEffect_( _layerIndex, _effectIndex );

        if( effect == nullptr )
        {
            return false;
        }

        effect->altitude = _altitude;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::setEffectHighlight( uint32_t _layerIndex, uint32_t _effectIndex, const Color & _color )
    {
        FontEffectStyleDesc * effect = this->getEffect_( _layerIndex, _effectIndex );

        if( effect == nullptr )
        {
            return false;
        }

        effect->highlight = _color;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::setEffectShadow( uint32_t _layerIndex, uint32_t _effectIndex, const Color & _color )
    {
        FontEffectStyleDesc * effect = this->getEffect_( _layerIndex, _effectIndex );

        if( effect == nullptr )
        {
            return false;
        }

        effect->shadow = _color;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::setEffectGradientEnabled( uint32_t _layerIndex, uint32_t _effectIndex, bool _enabled )
    {
        FontEffectStyleDesc * effect = this->getEffect_( _layerIndex, _effectIndex );

        if( effect == nullptr )
        {
            return false;
        }

        effect->gradient.enabled = _enabled;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::setEffectGradientAngle( uint32_t _layerIndex, uint32_t _effectIndex, float _angle )
    {
        FontEffectStyleDesc * effect = this->getEffect_( _layerIndex, _effectIndex );

        if( effect == nullptr )
        {
            return false;
        }

        effect->gradient.angle = _angle;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::setEffectGradientSpace( uint32_t _layerIndex, uint32_t _effectIndex, EFontEffectGradientSpace _space )
    {
        FontEffectStyleDesc * effect = this->getEffect_( _layerIndex, _effectIndex );

        if( effect == nullptr )
        {
            return false;
        }

        if( _space >= __EFEGS_MAX__ )
        {
            LOGGER_ERROR( "font effect invalid gradient space %u"
                , _space
            );

            return false;
        }

        effect->gradient.space = _space;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::addEffectGradientStop( uint32_t _layerIndex, uint32_t _effectIndex, float _t, const Color & _color )
    {
        FontEffectStyleDesc * effect = this->getEffect_( _layerIndex, _effectIndex );

        if( effect == nullptr )
        {
            return false;
        }

        VectorFontEffectGradientStops & stops = effect->gradient.stops;

        if( stops.empty() == false && _t < stops.back().t )
        {
            LOGGER_ERROR( "font effect gradient stops unsorted %f < %f"
                , _t
                , stops.back().t
            );

            return false;
        }

        FontEffectGradientStop stop;
        stop.t = _t;
        stop.color = _color;

        stops.emplace_back( stop );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
