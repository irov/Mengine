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
            case EFET_FILL:
                return 0.f;
            case EFET_OUTLINE:
                {
                    switch( _effect.position )
                    {
                    case EFEOP_OUTSIDE:
                        return _effect.width + _effect.sharpness + 1.f;
                    case EFEOP_CENTER:
                        return _effect.width * 0.5f + _effect.sharpness + 1.f;
                    case EFEOP_INSIDE:
                        return _effect.sharpness + 1.f;
                    }

                    return 0.f;
                }
            case EFET_SHADOW:
            case EFET_INNER_SHADOW:
                {
                    float offset_x = StdMath::fabsf( _effect.offset.x );
                    float offset_y = StdMath::fabsf( _effect.offset.y );

                    float extent = offset_x + offset_y + _effect.spread + 3.f * _effect.blur + 1.f;

                    return extent;
                }
            case EFET_GLOW:
            case EFET_INNER_GLOW:
                return _effect.spread + 3.f * _effect.blur + 1.f;
            case EFET_BEVEL:
                return _effect.size + _effect.soften + 1.f;
            case EFET_BLUR:
                return 3.f * _effect.blur + 1.f;
            case EFET_SATIN:
                {
                    float distance = StdMath::fabsf( _effect.distance );

                    float extent = distance + 3.f * _effect.blur + 1.f;

                    return extent;
                }
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
        static bool setRange( float * const _field, float _value, float _min, float _max, const Char * _name )
        {
            if( (_value >= _min) == false )
            {
                LOGGER_ERROR( "font effect invalid %s %f"
                    , _name
                    , _value
                );

                return false;
            }

            if( (_value <= _max) == false )
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
        static bool setEnum( uint32_t * const _field, uint32_t _value, uint32_t _max, const Char * _name )
        {
            if( _value >= _max )
            {
                LOGGER_ERROR( "font effect invalid %s %u"
                    , _name
                    , _value
                );

                return false;
            }

            *_field = _value;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool setAngle( float * const _field, float _value, const Char * _name )
        {
            float angle;
            if( setRange( &angle, _value, -MENGINE_FONTEFFECT_MAX_ANGLE, MENGINE_FONTEFFECT_MAX_ANGLE, _name ) == false )
            {
                return false;
            }

            float normalized = StdMath::fmodf( angle, 360.f );

            if( normalized < 0.f )
            {
                normalized += 360.f;
            }

            *_field = normalized;

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

        if( layoutCount > MENGINE_FONTEFFECT_MAX_LAYERS )
        {
            LOGGER_ERROR( "font effect invalid desc: %u enabled layers, max %u"
                , layoutCount
                , MENGINE_FONTEFFECT_MAX_LAYERS
            );

            return false;
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
    bool FontEffectBase::setupEffectCommon_( uint32_t _layerIndex, uint32_t _effectIndex, const FontEffectStyleDesc & _effect )
    {
        if( this->setEffectEnabled( _layerIndex, _effectIndex, _effect.enabled ) == false )
        {
            return false;
        }

        if( this->setEffectOpacity( _layerIndex, _effectIndex, _effect.opacity ) == false )
        {
            return false;
        }

        if( this->setEffectColor( _layerIndex, _effectIndex, _effect.color ) == false )
        {
            return false;
        }

        if( this->setEffectWidth( _layerIndex, _effectIndex, _effect.width ) == false )
        {
            return false;
        }

        if( this->setEffectSharpness( _layerIndex, _effectIndex, _effect.sharpness ) == false )
        {
            return false;
        }

        if( this->setEffectOutlinePosition( _layerIndex, _effectIndex, _effect.position ) == false )
        {
            return false;
        }

        if( this->setEffectOffset( _layerIndex, _effectIndex, _effect.offset ) == false )
        {
            return false;
        }

        if( this->setEffectBlur( _layerIndex, _effectIndex, _effect.blur ) == false )
        {
            return false;
        }

        if( this->setEffectSpread( _layerIndex, _effectIndex, _effect.spread ) == false )
        {
            return false;
        }

        if( this->setEffectDistance( _layerIndex, _effectIndex, _effect.distance ) == false )
        {
            return false;
        }

        if( this->setEffectInvert( _layerIndex, _effectIndex, _effect.invert ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::setupEffectBevel_( uint32_t _layerIndex, uint32_t _effectIndex, const FontEffectStyleDesc & _effect )
    {
        if( this->setEffectDepth( _layerIndex, _effectIndex, _effect.depth ) == false )
        {
            return false;
        }

        if( this->setEffectSize( _layerIndex, _effectIndex, _effect.size ) == false )
        {
            return false;
        }

        if( this->setEffectSoften( _layerIndex, _effectIndex, _effect.soften ) == false )
        {
            return false;
        }

        if( this->setEffectAngle( _layerIndex, _effectIndex, _effect.angle ) == false )
        {
            return false;
        }

        if( this->setEffectAltitude( _layerIndex, _effectIndex, _effect.altitude ) == false )
        {
            return false;
        }

        if( this->setEffectHighlight( _layerIndex, _effectIndex, _effect.highlight ) == false )
        {
            return false;
        }

        if( this->setEffectShadow( _layerIndex, _effectIndex, _effect.shadow ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::setupEffectGradient_( uint32_t _layerIndex, uint32_t _effectIndex, const FontEffectGradientDesc & _gradient )
    {
        if( this->setEffectGradientEnabled( _layerIndex, _effectIndex, _gradient.enabled ) == false )
        {
            return false;
        }

        if( this->setEffectGradientType( _layerIndex, _effectIndex, _gradient.type ) == false )
        {
            return false;
        }

        if( this->setEffectGradientAngle( _layerIndex, _effectIndex, _gradient.angle ) == false )
        {
            return false;
        }

        if( this->setEffectGradientCenter( _layerIndex, _effectIndex, _gradient.center ) == false )
        {
            return false;
        }

        if( this->setEffectGradientScale( _layerIndex, _effectIndex, _gradient.scale ) == false )
        {
            return false;
        }

        if( this->setEffectGradientReverse( _layerIndex, _effectIndex, _gradient.reverse ) == false )
        {
            return false;
        }

        if( this->setEffectGradientDither( _layerIndex, _effectIndex, _gradient.dither ) == false )
        {
            return false;
        }

        if( this->setEffectGradientSpace( _layerIndex, _effectIndex, _gradient.space ) == false )
        {
            return false;
        }

        for( const FontEffectGradientStop & stop : _gradient.stops )
        {
            if( this->addEffectGradientStop( _layerIndex, _effectIndex, stop.t, stop.color ) == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::setupDesc( const FontEffectDesc & _desc )
    {
        m_desc.layers.clear();

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

                if( this->setupEffectCommon_( layerIndex, effectIndex, effect ) == false )
                {
                    return false;
                }

                if( this->setupEffectBevel_( layerIndex, effectIndex, effect ) == false )
                {
                    return false;
                }

                if( this->setupEffectGradient_( layerIndex, effectIndex, effect.gradient ) == false )
                {
                    return false;
                }
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectBase::_release()
    {
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
    void FontEffectBase::makeGradientContext_( const FontEffectGradientDesc & _gradient, uint32_t _width, uint32_t _rows, int32_t _top, uint32_t _height, FontEffectGradientContext * const _context ) const
    {
        float sample = (float)m_effectSample;
        float padding = (float)m_padding;

        float angleRad = _gradient.angle * mt::constant::deg2rad;

        float dirX = StdMath::cosf( angleRad );
        float dirY = StdMath::sinf( angleRad );

        float glyphWidth = (float)_width;
        float glyphHeight = (float)_rows;

        float centerX = padding + glyphWidth * 0.5f;
        float centerY;
        float extent;

        if( _gradient.space == EFES_FONT )
        {
            float fontHeight = (float)_height * sample;

            centerY = padding + (float)_top - fontHeight * 0.5f;

            extent = StdMath::fabsf( glyphWidth * dirX ) + StdMath::fabsf( fontHeight * dirY );
        }
        else
        {
            centerY = padding + glyphHeight * 0.5f;

            extent = StdMath::fabsf( glyphWidth * dirX ) + StdMath::fabsf( glyphHeight * dirY );
        }

        float spaceHeight = (_gradient.space == EFES_FONT) ? (float)_height * sample : glyphHeight;

        float radius = 0.5f * StdMath::sqrtf( glyphWidth * glyphWidth + spaceHeight * spaceHeight );

        centerX += _gradient.center.x * glyphWidth * 0.5f;
        centerY += _gradient.center.y * spaceHeight * 0.5f;

        _context->dirX = dirX;
        _context->dirY = dirY;
        _context->angle = angleRad;
        _context->centerX = centerX;
        _context->centerY = centerY;
        _context->extentInv = (extent > 0.f) ? 1.f / extent : 1.f;
        _context->radiusInv = (radius > 0.f) ? 1.f / radius : 1.f;
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
                    bool useGradient = (effect.gradient.enabled == true && effect.gradient.stops.empty() == false);

                    FontEffectGradientContext context;
                    FontEffectPlane gradientSdf;

                    if( useGradient == true )
                    {
                        this->makeGradientContext_( effect.gradient, _width, _rows, _top, _height, &context );

                        if( effect.gradient.type == EFEGT_DISTANCE )
                        {
                            this->ensureSignedDistance_();

                            gradientSdf = m_scratch.getPlane( Detail::EFESP_SDF );
                        }
                    }

                    if( _channel == 4 && useGradient == true )
                    {
                        Helper::fontEffectTintImageGradient( source, gradientSdf, effect.gradient, context, effect.opacity, effectImage );
                    }
                    else if( _channel == 4 )
                    {
                        Helper::fontEffectTintImage( source, effect.color, effect.opacity, effectImage );
                    }
                    else if( useGradient == true )
                    {
                        Helper::fontEffectColorizeGradient( alpha, gradientSdf, effect.gradient, context, effect.opacity, effectImage );
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

                    float width = effect.width * sample;

                    float inner = 0.f;
                    float outer = width;

                    if( effect.position == EFEOP_CENTER )
                    {
                        inner = -width * 0.5f;
                        outer = width * 0.5f;
                    }
                    else if( effect.position == EFEOP_INSIDE )
                    {
                        inner = -width;
                        outer = 0.f;
                    }

                    Helper::fontEffectCoverageBand( sdf, inner, outer, effect.sharpness * sample, work0 );
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
                    if( effect.spread > 0.f )
                    {
                        this->ensureSignedDistance_();

                        FontEffectPlane sdf = m_scratch.getPlane( Detail::EFESP_SDF );

                        Helper::fontEffectCoverage( sdf, -effect.spread * sample, 0.f, work1 );
                        Helper::fontEffectInvert( work1, work0 );
                    }
                    else
                    {
                        Helper::fontEffectInvert( alpha, work0 );
                    }

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
            case EFET_SATIN:
                {
                    Helper::fontEffectSatin( alpha, effect, sample, work0, work1, tmp0, effectImage );
                    Helper::fontEffectCompositeOver( accumulator, effectImage );
                }break;
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

        if( _type >= MENGINE_FONTEFFECT_TYPE_MAX )
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

        mt::vec2f offset;
        if( Detail::setRange( &offset.x, _offset.x, -MENGINE_FONTEFFECT_MAX_OFFSET, MENGINE_FONTEFFECT_MAX_OFFSET, "effect offset x" ) == false )
        {
            return false;
        }

        if( Detail::setRange( &offset.y, _offset.y, -MENGINE_FONTEFFECT_MAX_OFFSET, MENGINE_FONTEFFECT_MAX_OFFSET, "effect offset y" ) == false )
        {
            return false;
        }

        effect->offset = offset;

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

        bool successful = Detail::setRange( &effect->depth, _depth, -MENGINE_FONTEFFECT_MAX_SIZE, MENGINE_FONTEFFECT_MAX_SIZE, "effect depth" );

        return successful;
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

        bool successful = Detail::setAngle( &effect->angle, _angle, "effect angle" );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::setEffectAltitude( uint32_t _layerIndex, uint32_t _effectIndex, float _altitude )
    {
        FontEffectStyleDesc * effect = this->getEffect_( _layerIndex, _effectIndex );

        if( effect == nullptr )
        {
            return false;
        }

        bool successful = Detail::setRange( &effect->altitude, _altitude, 0.f, 90.f, "effect altitude" );

        return successful;
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

        bool successful = Detail::setAngle( &effect->gradient.angle, _angle, "effect gradient angle" );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::setEffectGradientSpace( uint32_t _layerIndex, uint32_t _effectIndex, EFontEffectSpace _space )
    {
        FontEffectStyleDesc * effect = this->getEffect_( _layerIndex, _effectIndex );

        if( effect == nullptr )
        {
            return false;
        }

        if( _space >= MENGINE_FONTEFFECT_SPACE_MAX )
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
    bool FontEffectBase::setEffectOutlinePosition( uint32_t _layerIndex, uint32_t _effectIndex, EFontEffectOutlinePosition _position )
    {
        FontEffectStyleDesc * effect = this->getEffect_( _layerIndex, _effectIndex );

        if( effect == nullptr )
        {
            return false;
        }

        bool successful = Detail::setEnum( (uint32_t *)&effect->position, _position, MENGINE_FONTEFFECT_OUTLINE_POSITION_MAX, "effect outline position" );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::setEffectDistance( uint32_t _layerIndex, uint32_t _effectIndex, float _distance )
    {
        FontEffectStyleDesc * effect = this->getEffect_( _layerIndex, _effectIndex );

        if( effect == nullptr )
        {
            return false;
        }

        bool successful = Detail::setRange( &effect->distance, _distance, -MENGINE_FONTEFFECT_MAX_OFFSET, MENGINE_FONTEFFECT_MAX_OFFSET, "effect distance" );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::setEffectInvert( uint32_t _layerIndex, uint32_t _effectIndex, bool _invert )
    {
        FontEffectStyleDesc * effect = this->getEffect_( _layerIndex, _effectIndex );

        if( effect == nullptr )
        {
            return false;
        }

        effect->invert = _invert;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::setEffectGradientType( uint32_t _layerIndex, uint32_t _effectIndex, EFontEffectGradientType _type )
    {
        FontEffectStyleDesc * effect = this->getEffect_( _layerIndex, _effectIndex );

        if( effect == nullptr )
        {
            return false;
        }

        bool successful = Detail::setEnum( (uint32_t *)&effect->gradient.type, _type, MENGINE_FONTEFFECT_GRADIENT_TYPE_MAX, "effect gradient type" );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::setEffectGradientCenter( uint32_t _layerIndex, uint32_t _effectIndex, const mt::vec2f & _center )
    {
        FontEffectStyleDesc * effect = this->getEffect_( _layerIndex, _effectIndex );

        if( effect == nullptr )
        {
            return false;
        }

        mt::vec2f center;
        if( Detail::setRange( &center.x, _center.x, -1.f, 1.f, "effect gradient center x" ) == false )
        {
            return false;
        }

        if( Detail::setRange( &center.y, _center.y, -1.f, 1.f, "effect gradient center y" ) == false )
        {
            return false;
        }

        effect->gradient.center = center;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::setEffectGradientScale( uint32_t _layerIndex, uint32_t _effectIndex, float _scale )
    {
        FontEffectStyleDesc * effect = this->getEffect_( _layerIndex, _effectIndex );

        if( effect == nullptr )
        {
            return false;
        }

        bool successful = Detail::setRange( &effect->gradient.scale, _scale, 0.f, MENGINE_FONTEFFECT_MAX_SIZE, "effect gradient scale" );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::setEffectGradientReverse( uint32_t _layerIndex, uint32_t _effectIndex, bool _reverse )
    {
        FontEffectStyleDesc * effect = this->getEffect_( _layerIndex, _effectIndex );

        if( effect == nullptr )
        {
            return false;
        }

        effect->gradient.reverse = _reverse;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectBase::setEffectGradientDither( uint32_t _layerIndex, uint32_t _effectIndex, bool _dither )
    {
        FontEffectStyleDesc * effect = this->getEffect_( _layerIndex, _effectIndex );

        if( effect == nullptr )
        {
            return false;
        }

        effect->gradient.dither = _dither;

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

        float t;
        if( Detail::setRange( &t, _t, 0.f, 1.f, "effect gradient stop" ) == false )
        {
            return false;
        }

        VectorFontEffectGradientStops & stops = effect->gradient.stops;

        if( stops.empty() == false && t < stops.back().t )
        {
            LOGGER_ERROR( "font effect gradient stops unsorted %f < %f"
                , t
                , stops.back().t
            );

            return false;
        }

        FontEffectGradientStop stop;
        stop.t = t;
        stop.color = _color;

        stops.emplace_back( stop );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
