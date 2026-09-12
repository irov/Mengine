#include "FontEffectRaster.h"

#include "Config/StdAssert.h"
#include "Config/StdMath.h"
#include "Config/StdString.h"

#include "math/utils.h"
#include "math/constant.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FontEffectScratch::FontEffectScratch()
        : m_width( 0 )
        , m_height( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FontEffectScratch::~FontEffectScratch()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectScratch::setup( uint32_t _width, uint32_t _height )
    {
        m_width = _width;
        m_height = _height;

        size_t plane_size = (size_t)_width * (size_t)_height;
        size_t line_size = (size_t)(_width > _height ? _width : _height) + 2;

        for( uint32_t index = 0; index != MAX_PLANES; ++index )
        {
            VectorFontEffectPlaneBuffer & plane = m_planes[index];

            if( plane.size() < plane_size )
            {
                plane.resize( plane_size );
            }
        }

        for( uint32_t index = 0; index != MAX_IMAGES; ++index )
        {
            VectorFontEffectPlaneBuffer & image = m_images[index];

            if( image.size() < plane_size * 4 )
            {
                image.resize( plane_size * 4 );
            }
        }

        for( uint32_t index = 0; index != MAX_LINES; ++index )
        {
            VectorFontEffectPlaneBuffer & line = m_lines[index];

            if( line.size() < line_size )
            {
                line.resize( line_size );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectScratch::clear()
    {
        m_width = 0;
        m_height = 0;

        for( uint32_t index = 0; index != MAX_PLANES; ++index )
        {
            VectorFontEffectPlaneBuffer plane;
            m_planes[index].swap( plane );
        }

        for( uint32_t index = 0; index != MAX_IMAGES; ++index )
        {
            VectorFontEffectPlaneBuffer image;
            m_images[index].swap( image );
        }

        for( uint32_t index = 0; index != MAX_LINES; ++index )
        {
            VectorFontEffectPlaneBuffer line;
            m_lines[index].swap( line );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    FontEffectPlane FontEffectScratch::getPlane( uint32_t _index )
    {
        assert( _index < MAX_PLANES );

        FontEffectPlane plane;
        plane.width = m_width;
        plane.height = m_height;
        plane.data = m_planes[_index].data();

        return plane;
    }
    //////////////////////////////////////////////////////////////////////////
    FontEffectImage FontEffectScratch::getImage( uint32_t _index )
    {
        assert( _index < MAX_IMAGES );

        FontEffectImage image;
        image.width = m_width;
        image.height = m_height;
        image.data = m_images[_index].data();

        return image;
    }
    //////////////////////////////////////////////////////////////////////////
    float * FontEffectScratch::getLine( uint32_t _index )
    {
        assert( _index < MAX_LINES );

        float * line = m_lines[_index].data();

        return line;
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        namespace Detail
        {
            //////////////////////////////////////////////////////////////////////////
            static const float FONTEFFECT_EDT_INF = 1e20f;
            //////////////////////////////////////////////////////////////////////////
            constexpr uint32_t FONTEFFECT_BLUR_FIR_MAX_RADIUS = 8;
            constexpr uint32_t FONTEFFECT_BLUR_BOX_PASSES = 4;
            //////////////////////////////////////////////////////////////////////////
            static MENGINE_INLINE float clamp01( float _value )
            {
                float value = mt::clampf( 0.f, _value, 1.f );

                return value;
            }
            //////////////////////////////////////////////////////////////////////////
            static const uint8_t FONTEFFECT_BAYER8[64] = {
                 0, 32,  8, 40,  2, 34, 10, 42,
                48, 16, 56, 24, 50, 18, 58, 26,
                12, 44,  4, 36, 14, 46,  6, 38,
                60, 28, 52, 20, 62, 30, 54, 22,
                 3, 35, 11, 43,  1, 33,  9, 41,
                51, 19, 59, 27, 49, 17, 57, 25,
                15, 47,  7, 39, 13, 45,  5, 37,
                63, 31, 55, 23, 61, 29, 53, 21
            };
            //////////////////////////////////////////////////////////////////////////
            static MENGINE_INLINE float bayer8( uint32_t _x, uint32_t _y )
            {
                uint8_t value = FONTEFFECT_BAYER8[(_y % 8) * 8 + (_x % 8)];

                float dither = (float)value * (1.f / 64.f) - 0.5f;

                return dither;
            }
            //////////////////////////////////////////////////////////////////////////
            static float wrapPattern( float _value, EFontEffectPatternTile _tile )
            {
                switch( _tile )
                {
                case EFEPT_TILE:
                    return _value - StdMath::floorf( _value );
                case EFEPT_MIRROR:
                    {
                        float half = _value * 0.5f;
                        float fraction = half - StdMath::floorf( half );

                        return 1.f - StdMath::fabsf( 2.f * fraction - 1.f );
                    }
                case EFEPT_CLAMP:
                case EFEPT_STRETCH:
                    break;
                }

                return clamp01( _value );
            }
            //////////////////////////////////////////////////////////////////////////
            static void samplePattern( const FontEffectPatternImage & _pattern, float _u, float _v, float * const _rgba )
            {
                float x = _u * (float)_pattern.width - 0.5f;
                float y = _v * (float)_pattern.height - 0.5f;

                int32_t x0 = (int32_t)StdMath::floorf( x );
                int32_t y0 = (int32_t)StdMath::floorf( y );

                float fx = x - (float)x0;
                float fy = y - (float)y0;

                int32_t maxX = (int32_t)_pattern.width - 1;
                int32_t maxY = (int32_t)_pattern.height - 1;

                int32_t cx0 = (x0 < 0) ? 0 : ((x0 > maxX) ? maxX : x0);
                int32_t cy0 = (y0 < 0) ? 0 : ((y0 > maxY) ? maxY : y0);
                int32_t cx1 = (x0 + 1 < 0) ? 0 : ((x0 + 1 > maxX) ? maxX : x0 + 1);
                int32_t cy1 = (y0 + 1 < 0) ? 0 : ((y0 + 1 > maxY) ? maxY : y0 + 1);

                const float * p00 = _pattern.data.data() + ((size_t)cy0 * (size_t)_pattern.width + (size_t)cx0) * 4;
                const float * p10 = _pattern.data.data() + ((size_t)cy0 * (size_t)_pattern.width + (size_t)cx1) * 4;
                const float * p01 = _pattern.data.data() + ((size_t)cy1 * (size_t)_pattern.width + (size_t)cx0) * 4;
                const float * p11 = _pattern.data.data() + ((size_t)cy1 * (size_t)_pattern.width + (size_t)cx1) * 4;

                for( uint32_t channel = 0; channel != 4; ++channel )
                {
                    float top = p00[channel] + (p10[channel] - p00[channel]) * fx;
                    float bottom = p01[channel] + (p11[channel] - p01[channel]) * fx;

                    _rgba[channel] = top + (bottom - top) * fy;
                }
            }
            //////////////////////////////////////////////////////////////////////////
            static float blendChannel( EFontEffectBlendMode _mode, float _cb, float _cs )
            {
                switch( _mode )
                {
                case EFEBM_MULTIPLY:
                    return _cb * _cs;
                case EFEBM_SCREEN:
                    return _cb + _cs - _cb * _cs;
                case EFEBM_OVERLAY:
                    {
                        if( _cb <= 0.5f )
                        {
                            return 2.f * _cb * _cs;
                        }

                        return 1.f - 2.f * (1.f - _cb) * (1.f - _cs);
                    }
                case EFEBM_DARKEN:
                    return (_cb < _cs) ? _cb : _cs;
                case EFEBM_LIGHTEN:
                    return (_cb > _cs) ? _cb : _cs;
                case EFEBM_ADD:
                    {
                        float sum = _cb + _cs;

                        return (sum > 1.f) ? 1.f : sum;
                    }
                case EFEBM_SUBTRACT:
                    {
                        float difference = _cb - _cs;

                        return (difference < 0.f) ? 0.f : difference;
                    }
                case EFEBM_DIFFERENCE:
                    return StdMath::fabsf( _cb - _cs );
                case EFEBM_NORMAL:
                case EFEBM_ERASE:
                    break;
                }

                return _cs;
            }
            //////////////////////////////////////////////////////////////////////////
            static float gradientT( const FontEffectGradientDesc & _gradient, const FontEffectGradientContext & _context, uint32_t _x, uint32_t _y, float _sdf )
            {
                float dx = ((float)_x + 0.5f) - _context.centerX;
                float dy = ((float)_y + 0.5f) - _context.centerY;

                float scaleInv = (_gradient.scale > 0.f) ? 1.f / _gradient.scale : 1.f;

                float t = 0.f;

                switch( _gradient.type )
                {
                case EFEGT_LINEAR:
                    {
                        t = (dx * _context.dirX + dy * _context.dirY) * _context.extentInv * scaleInv + 0.5f;
                    }break;
                case EFEGT_REFLECTED:
                    {
                        float projection = (dx * _context.dirX + dy * _context.dirY) * _context.extentInv * scaleInv;

                        t = StdMath::fabsf( projection ) * 2.f;
                    }break;
                case EFEGT_RADIAL:
                    {
                        float distance = StdMath::sqrtf( dx * dx + dy * dy );

                        t = distance * _context.radiusInv * scaleInv;
                    }break;
                case EFEGT_ANGLE:
                    {
                        float direction = StdMath::atan2f( dy, dx ) - _context.angle;

                        float turn = direction * mt::constant::inv_two_pi + 0.5f;

                        t = turn - StdMath::floorf( turn );
                    }break;
                case EFEGT_DISTANCE:
                    {
                        t = -_sdf * _context.radiusInv * scaleInv;
                    }break;
                }

                if( _gradient.reverse == true )
                {
                    t = 1.f - t;
                }

                return t;
            }
            //////////////////////////////////////////////////////////////////////////
            // Felzenszwalb & Huttenlocher 1D squared euclidean distance transform
            static void edt1d( const float * _f, uint32_t _n, float * _d, float * _z, float * _v )
            {
                int32_t k = 0;
                _v[0] = 0.f;
                _z[0] = -FONTEFFECT_EDT_INF;
                _z[1] = +FONTEFFECT_EDT_INF;

                for( uint32_t q = 1; q != _n; ++q )
                {
                    float fq = _f[q];
                    float qf = (float)q;

                    float s;

                    for( ;; )
                    {
                        uint32_t vk = (uint32_t)_v[k];
                        float vkf = (float)vk;

                        s = ((fq + qf * qf) - (_f[vk] + vkf * vkf)) / (2.f * qf - 2.f * vkf);

                        if( s <= _z[k] && k > 0 )
                        {
                            --k;

                            continue;
                        }

                        break;
                    }

                    ++k;
                    _v[k] = qf;
                    _z[k] = s;
                    _z[k + 1] = +FONTEFFECT_EDT_INF;
                }

                k = 0;
                for( uint32_t q = 0; q != _n; ++q )
                {
                    float qf = (float)q;

                    while( _z[k + 1] < qf )
                    {
                        ++k;
                    }

                    uint32_t vk = (uint32_t)_v[k];
                    float vkf = (float)vk;

                    _d[q] = (qf - vkf) * (qf - vkf) + _f[vk];
                }
            }
            //////////////////////////////////////////////////////////////////////////
            // 2D squared euclidean distance transform, in place: _plane holds 0 for seed pixels and INF otherwise
            static void edt2d( const FontEffectPlane & _plane, float * _lineF, float * _lineD, float * _lineZ, float * _lineV )
            {
                uint32_t width = _plane.width;
                uint32_t height = _plane.height;
                float * data = _plane.data;

                for( uint32_t x = 0; x != width; ++x )
                {
                    for( uint32_t y = 0; y != height; ++y )
                    {
                        _lineF[y] = data[y * width + x];
                    }

                    Detail::edt1d( _lineF, height, _lineD, _lineZ, _lineV );

                    for( uint32_t y = 0; y != height; ++y )
                    {
                        data[y * width + x] = _lineD[y];
                    }
                }

                for( uint32_t y = 0; y != height; ++y )
                {
                    float * row = data + y * width;

                    for( uint32_t x = 0; x != width; ++x )
                    {
                        _lineF[x] = row[x];
                    }

                    Detail::edt1d( _lineF, width, _lineD, _lineZ, _lineV );

                    for( uint32_t x = 0; x != width; ++x )
                    {
                        row[x] = _lineD[x];
                    }
                }
            }
            //////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////
            static void boxBlurLineFractional( const float * _src, uint32_t _n, uint32_t _stride, float _radius, float * _dst, uint32_t _dstStride )
            {
                int32_t radius = (int32_t)StdMath::floorf( _radius );
                float fraction = _radius - (float)radius;

                float window = (float)(radius * 2 + 1) + 2.f * fraction;
                float windowInv = 1.f / window;

                float acc = 0.f;

                for( int32_t index = -radius; index <= radius; ++index )
                {
                    if( index < 0 )
                    {
                        continue;
                    }

                    if( index >= (int32_t)_n )
                    {
                        continue;
                    }

                    acc += _src[(uint32_t)index * _stride];
                }

                for( uint32_t i = 0; i != _n; ++i )
                {
                    float edge = 0.f;

                    int32_t left = (int32_t)i - radius - 1;
                    int32_t right = (int32_t)i + radius + 1;

                    if( left >= 0 )
                    {
                        edge += _src[(uint32_t)left * _stride];
                    }

                    if( right < (int32_t)_n )
                    {
                        edge += _src[(uint32_t)right * _stride];
                    }

                    _dst[i * _dstStride] = (acc + edge * fraction) * windowInv;

                    int32_t add_index = (int32_t)i + radius + 1;
                    int32_t sub_index = (int32_t)i - radius;

                    if( add_index < (int32_t)_n )
                    {
                        acc += _src[(uint32_t)add_index * _stride];
                    }

                    if( sub_index >= 0 )
                    {
                        acc -= _src[(uint32_t)sub_index * _stride];
                    }
                }
            }
            //////////////////////////////////////////////////////////////////////////
            static void firBlurLine( const float * _src, uint32_t _n, uint32_t _stride, const float * _kernel, int32_t _radius, float * _dst, uint32_t _dstStride )
            {
                for( uint32_t i = 0; i != _n; ++i )
                {
                    float acc = 0.f;

                    for( int32_t k = -_radius; k <= _radius; ++k )
                    {
                        int32_t index = (int32_t)i + k;

                        if( index < 0 )
                        {
                            continue;
                        }

                        if( index >= (int32_t)_n )
                        {
                            continue;
                        }

                        acc += _src[(uint32_t)index * _stride] * _kernel[k + _radius];
                    }

                    _dst[i * _dstStride] = acc;
                }
            }
            //////////////////////////////////////////////////////////////////////////
            static void boxBlurPlaneFractional( const FontEffectPlane & _plane, float _radius, const FontEffectPlane & _tmp )
            {
                uint32_t width = _plane.width;
                uint32_t height = _plane.height;

                for( uint32_t y = 0; y != height; ++y )
                {
                    Detail::boxBlurLineFractional( _plane.data + y * width, width, 1, _radius, _tmp.data + y * width, 1 );
                }

                for( uint32_t x = 0; x != width; ++x )
                {
                    Detail::boxBlurLineFractional( _tmp.data + x, height, width, _radius, _plane.data + x, width );
                }
            }
            //////////////////////////////////////////////////////////////////////////
            static void firBlurPlane( const FontEffectPlane & _plane, const float * _kernel, int32_t _radius, const FontEffectPlane & _tmp )
            {
                uint32_t width = _plane.width;
                uint32_t height = _plane.height;

                for( uint32_t y = 0; y != height; ++y )
                {
                    Detail::firBlurLine( _plane.data + y * width, width, 1, _kernel, _radius, _tmp.data + y * width, 1 );
                }

                for( uint32_t x = 0; x != width; ++x )
                {
                    Detail::firBlurLine( _tmp.data + x, height, width, _kernel, _radius, _plane.data + x, width );
                }
            }
            //////////////////////////////////////////////////////////////////////////
            static void evaluateGradient( const FontEffectGradientDesc & _gradient, float _t, float * const _rgba )
            {
                const VectorFontEffectGradientStops & stops = _gradient.stops;

                VectorFontEffectGradientStops::size_type stops_count = stops.size();

                if( stops_count == 0 )
                {
                    _rgba[0] = 1.f;
                    _rgba[1] = 1.f;
                    _rgba[2] = 1.f;
                    _rgba[3] = 1.f;

                    return;
                }

                const FontEffectGradientStop & first = stops.front();

                if( stops_count == 1 || _t <= first.t )
                {
                    _rgba[0] = first.color.getR();
                    _rgba[1] = first.color.getG();
                    _rgba[2] = first.color.getB();
                    _rgba[3] = first.color.getA();

                    return;
                }

                const FontEffectGradientStop & last = stops.back();

                if( _t >= last.t )
                {
                    _rgba[0] = last.color.getR();
                    _rgba[1] = last.color.getG();
                    _rgba[2] = last.color.getB();
                    _rgba[3] = last.color.getA();

                    return;
                }

                for( VectorFontEffectGradientStops::size_type index = 1; index != stops_count; ++index )
                {
                    const FontEffectGradientStop & s0 = stops[index - 1];
                    const FontEffectGradientStop & s1 = stops[index];

                    if( _t > s1.t )
                    {
                        continue;
                    }

                    float span = s1.t - s0.t;
                    float k = (span > 0.f) ? (_t - s0.t) / span : 1.f;

                    _rgba[0] = s0.color.getR() + (s1.color.getR() - s0.color.getR()) * k;
                    _rgba[1] = s0.color.getG() + (s1.color.getG() - s0.color.getG()) * k;
                    _rgba[2] = s0.color.getB() + (s1.color.getB() - s0.color.getB()) * k;
                    _rgba[3] = s0.color.getA() + (s1.color.getA() - s0.color.getA()) * k;

                    return;
                }

                _rgba[0] = last.color.getR();
                _rgba[1] = last.color.getG();
                _rgba[2] = last.color.getB();
                _rgba[3] = last.color.getA();
            }
            //////////////////////////////////////////////////////////////////////////
        }
        //////////////////////////////////////////////////////////////////////////
        void fontEffectPlaneClear( const FontEffectPlane & _plane )
        {
            size_t size = (size_t)_plane.width * (size_t)_plane.height;

            for( size_t index = 0; index != size; ++index )
            {
                _plane.data[index] = 0.f;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void fontEffectPlaneCopy( const FontEffectPlane & _src, const FontEffectPlane & _dst )
        {
            size_t size = (size_t)_src.width * (size_t)_src.height;

            StdString::memcpy( _dst.data, _src.data, size * sizeof( float ) );
        }
        //////////////////////////////////////////////////////////////////////////
        void fontEffectImageClear( const FontEffectImage & _image )
        {
            size_t size = (size_t)_image.width * (size_t)_image.height * 4;

            for( size_t index = 0; index != size; ++index )
            {
                _image.data[index] = 0.f;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void fontEffectImageCopy( const FontEffectImage & _src, const FontEffectImage & _dst )
        {
            size_t size = (size_t)_src.width * (size_t)_src.height * 4;

            StdString::memcpy( _dst.data, _src.data, size * sizeof( float ) );
        }
        //////////////////////////////////////////////////////////////////////////
        void fontEffectImportA8( const void * _buffer, uint32_t _width, uint32_t _rows, uint32_t _pitch, uint32_t _padding, const FontEffectPlane & _alpha )
        {
            Helper::fontEffectPlaneClear( _alpha );

            const uint8_t * src = (const uint8_t *)_buffer;

            for( uint32_t y = 0; y != _rows; ++y )
            {
                const uint8_t * src_row = src + (size_t)y * (size_t)_pitch;
                float * dst_row = _alpha.data + (size_t)(y + _padding) * (size_t)_alpha.width + _padding;

                for( uint32_t x = 0; x != _width; ++x )
                {
                    dst_row[x] = (float)src_row[x] * (1.f / 255.f);
                }
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void fontEffectImportBGRA( const void * _buffer, uint32_t _width, uint32_t _rows, uint32_t _pitch, uint32_t _padding, const FontEffectPlane & _alpha, const FontEffectImage & _image )
        {
            Helper::fontEffectPlaneClear( _alpha );
            Helper::fontEffectImageClear( _image );

            const uint8_t * src = (const uint8_t *)_buffer;

            for( uint32_t y = 0; y != _rows; ++y )
            {
                const uint8_t * src_row = src + (size_t)y * (size_t)_pitch;
                size_t dst_index = (size_t)(y + _padding) * (size_t)_alpha.width + _padding;

                float * dst_alpha = _alpha.data + dst_index;
                float * dst_image = _image.data + dst_index * 4;

                for( uint32_t x = 0; x != _width; ++x )
                {
                    const uint8_t * px = src_row + x * 4;

                    float b = (float)px[0] * (1.f / 255.f);
                    float g = (float)px[1] * (1.f / 255.f);
                    float r = (float)px[2] * (1.f / 255.f);
                    float a = (float)px[3] * (1.f / 255.f);

                    dst_alpha[x] = a;

                    dst_image[x * 4 + 0] = r;
                    dst_image[x * 4 + 1] = g;
                    dst_image[x * 4 + 2] = b;
                    dst_image[x * 4 + 3] = a;
                }
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void fontEffectSignedDistance( const FontEffectPlane & _alpha, const FontEffectPlane & _sdf, const FontEffectPlane & _tmp0, const FontEffectPlane & _tmp1, float * _lineF, float * _lineD, float * _lineZ, float * _lineV )
        {
            uint32_t width = _alpha.width;
            uint32_t height = _alpha.height;

            size_t size = (size_t)width * (size_t)height;

            for( size_t index = 0; index != size; ++index )
            {
                float a = _alpha.data[index];

                if( a >= 0.5f )
                {
                    float inner = a - 0.5f;

                    _tmp0.data[index] = 0.f;
                    _tmp1.data[index] = (a < 1.f) ? inner * inner : Detail::FONTEFFECT_EDT_INF;
                }
                else
                {
                    float outer = 0.5f - a;

                    _tmp0.data[index] = (a > 0.f) ? outer * outer : Detail::FONTEFFECT_EDT_INF;
                    _tmp1.data[index] = 0.f;
                }
            }

            Detail::edt2d( _tmp0, _lineF, _lineD, _lineZ, _lineV );
            Detail::edt2d( _tmp1, _lineF, _lineD, _lineZ, _lineV );

            for( size_t index = 0; index != size; ++index )
            {
                float dOut2 = _tmp0.data[index];
                float dIn2 = _tmp1.data[index];

                float dOut = StdMath::sqrtf( dOut2 );
                float dIn = StdMath::sqrtf( dIn2 );

                _sdf.data[index] = dOut - dIn;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void fontEffectCoverage( const FontEffectPlane & _sdf, float _width, float _sharpness, const FontEffectPlane & _out )
        {
            size_t size = (size_t)_sdf.width * (size_t)_sdf.height;

            float ramp = 1.f + _sharpness;
            float rampInv = 1.f / ramp;

            for( size_t index = 0; index != size; ++index )
            {
                float sdf = _sdf.data[index];

                float c = (_width - sdf) * rampInv + 0.5f;

                _out.data[index] = Detail::clamp01( c );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void fontEffectCoverageBand( const FontEffectPlane & _sdf, float _inner, float _outer, float _sharpness, const FontEffectPlane & _out )
        {
            size_t size = (size_t)_sdf.width * (size_t)_sdf.height;

            float ramp = 1.f + _sharpness;
            float rampInv = 1.f / ramp;

            for( size_t index = 0; index != size; ++index )
            {
                float sdf = _sdf.data[index];

                float outer = (_outer - sdf) * rampInv + 0.5f;
                float inner = (sdf - _inner) * rampInv + 0.5f;

                _out.data[index] = Detail::clamp01( outer ) * Detail::clamp01( inner );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void fontEffectInvert( const FontEffectPlane & _src, const FontEffectPlane & _dst )
        {
            size_t size = (size_t)_src.width * (size_t)_src.height;

            for( size_t index = 0; index != size; ++index )
            {
                _dst.data[index] = 1.f - _src.data[index];
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void fontEffectMultiply( const FontEffectPlane & _plane, const FontEffectPlane & _mask )
        {
            size_t size = (size_t)_plane.width * (size_t)_plane.height;

            for( size_t index = 0; index != size; ++index )
            {
                _plane.data[index] *= _mask.data[index];
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void fontEffectOffset( const FontEffectPlane & _src, float _dx, float _dy, const FontEffectPlane & _dst )
        {
            uint32_t width = _src.width;
            uint32_t height = _src.height;

            int32_t ix = (int32_t)StdMath::floorf( _dx );
            int32_t iy = (int32_t)StdMath::floorf( _dy );

            float fx = _dx - (float)ix;
            float fy = _dy - (float)iy;

            for( uint32_t y = 0; y != height; ++y )
            {
                float * dst_row = _dst.data + (size_t)y * (size_t)width;

                int32_t sy0 = (int32_t)y - iy - 1;
                int32_t sy1 = (int32_t)y - iy;

                const float * src_row0 = (sy0 >= 0 && sy0 < (int32_t)height) ? _src.data + (size_t)sy0 * (size_t)width : nullptr;
                const float * src_row1 = (sy1 >= 0 && sy1 < (int32_t)height) ? _src.data + (size_t)sy1 * (size_t)width : nullptr;

                for( uint32_t x = 0; x != width; ++x )
                {
                    int32_t sx0 = (int32_t)x - ix - 1;
                    int32_t sx1 = (int32_t)x - ix;

                    bool validX0 = (sx0 >= 0 && sx0 < (int32_t)width);
                    bool validX1 = (sx1 >= 0 && sx1 < (int32_t)width);

                    float v00 = (src_row0 != nullptr && validX0 == true) ? src_row0[sx0] : 0.f;
                    float v10 = (src_row0 != nullptr && validX1 == true) ? src_row0[sx1] : 0.f;
                    float v01 = (src_row1 != nullptr && validX0 == true) ? src_row1[sx0] : 0.f;
                    float v11 = (src_row1 != nullptr && validX1 == true) ? src_row1[sx1] : 0.f;

                    float top = v00 * fx + v10 * (1.f - fx);
                    float bottom = v01 * fx + v11 * (1.f - fx);

                    dst_row[x] = top * fy + bottom * (1.f - fy);
                }
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void fontEffectBlurPlane( const FontEffectPlane & _plane, float _sigma, const FontEffectPlane & _tmp )
        {
            if( _sigma <= 0.f )
            {
                return;
            }

            int32_t radius = (int32_t)StdMath::ceilf( 3.f * _sigma );

            if( radius <= (int32_t)Detail::FONTEFFECT_BLUR_FIR_MAX_RADIUS )
            {
                float kernel[Detail::FONTEFFECT_BLUR_FIR_MAX_RADIUS * 2 + 1];

                float sigmaInv = 1.f / (2.f * _sigma * _sigma);
                float total = 0.f;

                for( int32_t index = -radius; index <= radius; ++index )
                {
                    float offset = (float)index;

                    float weight = StdMath::expf( -offset * offset * sigmaInv );

                    kernel[index + radius] = weight;
                    total += weight;
                }

                float totalInv = 1.f / total;

                for( int32_t index = 0; index != radius * 2 + 1; ++index )
                {
                    kernel[index] *= totalInv;
                }

                Detail::firBlurPlane( _plane, kernel, radius, _tmp );

                return;
            }

            float boxRadius = (StdMath::sqrtf( 12.f * _sigma * _sigma / (float)Detail::FONTEFFECT_BLUR_BOX_PASSES + 1.f ) - 1.f) * 0.5f;

            for( uint32_t pass = 0; pass != Detail::FONTEFFECT_BLUR_BOX_PASSES; ++pass )
            {
                Detail::boxBlurPlaneFractional( _plane, boxRadius, _tmp );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void fontEffectBlurImage( const FontEffectImage & _image, float _sigma, const FontEffectPlane & _tmp0, const FontEffectPlane & _tmp1 )
        {
            if( _sigma <= 0.f )
            {
                return;
            }

            size_t size = (size_t)_image.width * (size_t)_image.height;

            for( uint32_t channel = 0; channel != 4; ++channel )
            {
                for( size_t index = 0; index != size; ++index )
                {
                    _tmp0.data[index] = _image.data[index * 4 + channel];
                }

                Helper::fontEffectBlurPlane( _tmp0, _sigma, _tmp1 );

                for( size_t index = 0; index != size; ++index )
                {
                    _image.data[index * 4 + channel] = _tmp0.data[index];
                }
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void fontEffectColorize( const FontEffectPlane & _coverage, const Color & _color, float _opacity, const FontEffectImage & _out )
        {
            size_t size = (size_t)_coverage.width * (size_t)_coverage.height;

            float r = _color.getR();
            float g = _color.getG();
            float b = _color.getB();
            float a = _color.getA() * _opacity;

            for( size_t index = 0; index != size; ++index )
            {
                float c = Detail::clamp01( _coverage.data[index] ) * a;

                float * px = _out.data + index * 4;

                px[0] = r * c;
                px[1] = g * c;
                px[2] = b * c;
                px[3] = c;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void fontEffectColorizeGradient( const FontEffectPlane & _coverage, const FontEffectPlane & _sdf, const FontEffectGradientDesc & _gradient, const FontEffectGradientContext & _context, float _opacity, const FontEffectImage & _out )
        {
            uint32_t width = _coverage.width;
            uint32_t height = _coverage.height;

            for( uint32_t y = 0; y != height; ++y )
            {
                for( uint32_t x = 0; x != width; ++x )
                {
                    size_t index = (size_t)y * (size_t)width + x;

                    float c = Detail::clamp01( _coverage.data[index] );

                    float * px = _out.data + index * 4;

                    if( c <= 0.f )
                    {
                        px[0] = 0.f;
                        px[1] = 0.f;
                        px[2] = 0.f;
                        px[3] = 0.f;

                        continue;
                    }

                    float sdf = (_sdf.data != nullptr) ? _sdf.data[index] : 0.f;

                    float t = Detail::gradientT( _gradient, _context, x, y, sdf );

                    float rgba[4];
                    Detail::evaluateGradient( _gradient, Detail::clamp01( t ), rgba );

                    if( _gradient.dither == true )
                    {
                        float dither = Detail::bayer8( x, y ) * (1.f / 255.f);

                        rgba[0] = Detail::clamp01( rgba[0] + dither );
                        rgba[1] = Detail::clamp01( rgba[1] + dither );
                        rgba[2] = Detail::clamp01( rgba[2] + dither );
                    }

                    float a = rgba[3] * _opacity * c;

                    px[0] = rgba[0] * a;
                    px[1] = rgba[1] * a;
                    px[2] = rgba[2] * a;
                    px[3] = a;
                }
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void fontEffectTintImage( const FontEffectImage & _src, const Color & _color, float _opacity, const FontEffectImage & _out )
        {
            size_t size = (size_t)_src.width * (size_t)_src.height;

            float r = _color.getR();
            float g = _color.getG();
            float b = _color.getB();
            float a = _color.getA() * _opacity;

            for( size_t index = 0; index != size; ++index )
            {
                const float * src = _src.data + index * 4;
                float * dst = _out.data + index * 4;

                dst[0] = src[0] * r * a;
                dst[1] = src[1] * g * a;
                dst[2] = src[2] * b * a;
                dst[3] = src[3] * a;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void fontEffectTintImageGradient( const FontEffectImage & _src, const FontEffectPlane & _sdf, const FontEffectGradientDesc & _gradient, const FontEffectGradientContext & _context, float _opacity, const FontEffectImage & _out )
        {
            uint32_t width = _src.width;
            uint32_t height = _src.height;

            for( uint32_t y = 0; y != height; ++y )
            {
                for( uint32_t x = 0; x != width; ++x )
                {
                    size_t index = (size_t)y * (size_t)width + x;

                    const float * src = _src.data + index * 4;
                    float * dst = _out.data + index * 4;

                    float sdf = (_sdf.data != nullptr) ? _sdf.data[index] : 0.f;

                    float t = Detail::gradientT( _gradient, _context, x, y, sdf );

                    float rgba[4];
                    Detail::evaluateGradient( _gradient, Detail::clamp01( t ), rgba );

                    float a = rgba[3] * _opacity;

                    dst[0] = src[0] * rgba[0] * a;
                    dst[1] = src[1] * rgba[1] * a;
                    dst[2] = src[2] * rgba[2] * a;
                    dst[3] = src[3] * a;
                }
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void fontEffectPattern( const FontEffectPlane & _coverage, const FontEffectPatternImage & _pattern, const FontEffectPatternContext & _context, EFontEffectPatternTile _tile, const Color & _color, float _opacity, const FontEffectImage & _out )
        {
            uint32_t width = _coverage.width;
            uint32_t height = _coverage.height;

            float r = _color.getR();
            float g = _color.getG();
            float b = _color.getB();
            float a = _color.getA() * _opacity;

            for( uint32_t y = 0; y != height; ++y )
            {
                for( uint32_t x = 0; x != width; ++x )
                {
                    size_t index = (size_t)y * (size_t)width + x;

                    float coverage = Detail::clamp01( _coverage.data[index] );

                    float * px = _out.data + index * 4;

                    if( coverage <= 0.f )
                    {
                        px[0] = 0.f;
                        px[1] = 0.f;
                        px[2] = 0.f;
                        px[3] = 0.f;

                        continue;
                    }

                    float dx = ((float)x + 0.5f) - _context.originX;
                    float dy = ((float)y + 0.5f) - _context.originY;

                    float rx = dx * _context.cosAngle + dy * _context.sinAngle;
                    float ry = dy * _context.cosAngle - dx * _context.sinAngle;

                    float u = Detail::wrapPattern( rx * _context.invWidth, _tile );
                    float v = Detail::wrapPattern( ry * _context.invHeight, _tile );

                    float rgba[4];
                    Detail::samplePattern( _pattern, u, v, rgba );

                    float alpha = rgba[3] * a * coverage;

                    px[0] = rgba[0] * r * a * coverage;
                    px[1] = rgba[1] * g * a * coverage;
                    px[2] = rgba[2] * b * a * coverage;
                    px[3] = alpha;
                }
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void fontEffectSatin( const FontEffectPlane & _alpha, const FontEffectStyleDesc & _effect, float _sample, const FontEffectPlane & _work0, const FontEffectPlane & _work1, const FontEffectPlane & _tmp0, const FontEffectImage & _out )
        {
            float angleRad = _effect.angle * mt::constant::deg2rad;

            float distance = _effect.distance * _sample;

            float dx = distance * StdMath::cosf( angleRad );
            float dy = -distance * StdMath::sinf( angleRad );

            Helper::fontEffectOffset( _alpha, dx, dy, _work0 );
            Helper::fontEffectOffset( _alpha, -dx, -dy, _work1 );

            size_t size = (size_t)_alpha.width * (size_t)_alpha.height;

            for( size_t index = 0; index != size; ++index )
            {
                float interference = StdMath::fabsf( _work0.data[index] - _work1.data[index] );

                _work0.data[index] = (_effect.invert == true) ? 1.f - interference : interference;
            }

            Helper::fontEffectBlurPlane( _work0, _effect.blur * _sample, _tmp0 );
            Helper::fontEffectMultiply( _work0, _alpha );
            Helper::fontEffectColorize( _work0, _effect.color, _effect.opacity, _out );
        }
        //////////////////////////////////////////////////////////////////////////
        void fontEffectBevel( const FontEffectPlane & _sdf, const FontEffectPlane & _alpha, const FontEffectStyleDesc & _effect, float _sample, const FontEffectPlane & _tmp0, const FontEffectPlane & _tmp1, const FontEffectImage & _out )
        {
            uint32_t width = _sdf.width;
            uint32_t height = _sdf.height;

            size_t size = (size_t)width * (size_t)height;

            float bevel_size = _effect.size * _sample;
            float bevel_size_inv = (bevel_size > 0.f) ? 1.f / bevel_size : 0.f;
            float depth = _effect.depth * _sample;

            for( size_t index = 0; index != size; ++index )
            {
                float sdf = _sdf.data[index];

                float h = Detail::clamp01( -sdf * bevel_size_inv ) * depth;

                _tmp0.data[index] = h;
            }

            Helper::fontEffectBlurPlane( _tmp0, _effect.soften * _sample, _tmp1 );

            float angle_rad = _effect.angle * mt::constant::deg2rad;
            float altitude_rad = _effect.altitude * mt::constant::deg2rad;

            float cos_alt = StdMath::cosf( altitude_rad );
            float sin_alt = StdMath::sinf( altitude_rad );

            float lx = cos_alt * StdMath::cosf( angle_rad );
            float ly = -cos_alt * StdMath::sinf( angle_rad );
            float lz = sin_alt;

            float norm = (cos_alt > 0.001f) ? 1.f / cos_alt : 1.f;

            float hr = _effect.highlight.getR();
            float hg = _effect.highlight.getG();
            float hb = _effect.highlight.getB();
            float ha = _effect.highlight.getA();

            float sr = _effect.shadow.getR();
            float sg = _effect.shadow.getG();
            float sb = _effect.shadow.getB();
            float sa = _effect.shadow.getA();

            for( uint32_t y = 0; y != height; ++y )
            {
                for( uint32_t x = 0; x != width; ++x )
                {
                    size_t index = (size_t)y * (size_t)width + x;

                    float * px = _out.data + index * 4;

                    float a = _alpha.data[index];

                    if( a <= 0.f )
                    {
                        px[0] = 0.f;
                        px[1] = 0.f;
                        px[2] = 0.f;
                        px[3] = 0.f;

                        continue;
                    }

                    uint32_t xl = (x > 0) ? x - 1 : x;
                    uint32_t xr = (x + 1 < width) ? x + 1 : x;
                    uint32_t yt = (y > 0) ? y - 1 : y;
                    uint32_t yb = (y + 1 < height) ? y + 1 : y;

                    float dhdx = (_tmp0.data[(size_t)y * width + xr] - _tmp0.data[(size_t)y * width + xl]) * 0.5f;
                    float dhdy = (_tmp0.data[(size_t)yb * width + x] - _tmp0.data[(size_t)yt * width + x]) * 0.5f;

                    float nx = -dhdx;
                    float ny = -dhdy;
                    float nz = 1.f;

                    float nl = StdMath::sqrtf( nx * nx + ny * ny + nz * nz );

                    nx /= nl;
                    ny /= nl;
                    nz /= nl;

                    float lambert = nx * lx + ny * ly + nz * lz;

                    float d = (lambert - sin_alt) * norm;

                    float hl = Detail::clamp01( d );
                    float sh = Detail::clamp01( -d );

                    float opacity = _effect.opacity * a;

                    float hla = ha * hl * opacity;
                    float sha = sa * sh * opacity;

                    px[0] = hr * hla + sr * sha;
                    px[1] = hg * hla + sg * sha;
                    px[2] = hb * hla + sb * sha;
                    px[3] = Detail::clamp01( hla + sha );
                }
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void fontEffectComposite( const FontEffectImage & _dst, const FontEffectImage & _src, EFontEffectBlendMode _mode )
        {
            size_t size = (size_t)_dst.width * (size_t)_dst.height;

            if( _mode == EFEBM_NORMAL )
            {
                for( size_t index = 0; index != size; ++index )
                {
                    const float * src = _src.data + index * 4;
                    float * dst = _dst.data + index * 4;

                    float ia = 1.f - src[3];

                    dst[0] = src[0] + dst[0] * ia;
                    dst[1] = src[1] + dst[1] * ia;
                    dst[2] = src[2] + dst[2] * ia;
                    dst[3] = src[3] + dst[3] * ia;
                }

                return;
            }

            if( _mode == EFEBM_ERASE )
            {
                for( size_t index = 0; index != size; ++index )
                {
                    const float * src = _src.data + index * 4;
                    float * dst = _dst.data + index * 4;

                    float ia = 1.f - src[3];

                    dst[0] *= ia;
                    dst[1] *= ia;
                    dst[2] *= ia;
                    dst[3] *= ia;
                }

                return;
            }

            for( size_t index = 0; index != size; ++index )
            {
                const float * src = _src.data + index * 4;
                float * dst = _dst.data + index * 4;

                float as = src[3];
                float ab = dst[3];

                float asInv = (as > 0.f) ? 1.f / as : 0.f;
                float abInv = (ab > 0.f) ? 1.f / ab : 0.f;

                float ao = as + ab * (1.f - as);

                for( uint32_t channel = 0; channel != 3; ++channel )
                {
                    float cs = src[channel] * asInv;
                    float cb = dst[channel] * abInv;

                    float blended = Detail::blendChannel( _mode, cb, cs );

                    dst[channel] = (1.f - ab) * as * cs + (1.f - as) * ab * cb + as * ab * blended;
                }

                dst[3] = ao;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void fontEffectKnockout( const FontEffectImage & _image, const FontEffectPlane & _mask )
        {
            size_t size = (size_t)_image.width * (size_t)_image.height;

            for( size_t index = 0; index != size; ++index )
            {
                float ia = 1.f - Detail::clamp01( _mask.data[index] );

                float * px = _image.data + index * 4;

                px[0] *= ia;
                px[1] *= ia;
                px[2] *= ia;
                px[3] *= ia;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void fontEffectImageOpacity( const FontEffectImage & _image, float _opacity )
        {
            if( _opacity == 1.f )
            {
                return;
            }

            size_t size = (size_t)_image.width * (size_t)_image.height * 4;

            for( size_t index = 0; index != size; ++index )
            {
                _image.data[index] *= _opacity;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        bool fontEffectTightCrop( const FontEffectImage & _image, FontEffectRect * const _rect )
        {
            uint32_t width = _image.width;
            uint32_t height = _image.height;

            uint32_t min_x = width;
            uint32_t min_y = height;
            uint32_t max_x = 0;
            uint32_t max_y = 0;

            bool found = false;

            for( uint32_t y = 0; y != height; ++y )
            {
                for( uint32_t x = 0; x != width; ++x )
                {
                    float a = _image.data[((size_t)y * width + x) * 4 + 3];

                    if( a * 255.f < 0.5f )
                    {
                        continue;
                    }

                    found = true;

                    if( x < min_x )
                    {
                        min_x = x;
                    }

                    if( x > max_x )
                    {
                        max_x = x;
                    }

                    if( y < min_y )
                    {
                        min_y = y;
                    }

                    if( y > max_y )
                    {
                        max_y = y;
                    }
                }
            }

            if( found == false )
            {
                _rect->x = 0;
                _rect->y = 0;
                _rect->width = 0;
                _rect->height = 0;

                return false;
            }

            _rect->x = min_x;
            _rect->y = min_y;
            _rect->width = max_x - min_x + 1;
            _rect->height = max_y - min_y + 1;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void fontEffectExport8( const FontEffectImage & _image, const FontEffectRect & _rect, bool _rgba, uint8_t * const _buffer )
        {
            uint32_t width = _image.width;

            for( uint32_t y = 0; y != _rect.height; ++y )
            {
                const float * src_row = _image.data + ((size_t)(_rect.y + y) * width + _rect.x) * 4;
                uint8_t * dst_row = _buffer + (size_t)y * (size_t)_rect.width * 4;

                for( uint32_t x = 0; x != _rect.width; ++x )
                {
                    const float * src = src_row + x * 4;
                    uint8_t * dst = dst_row + x * 4;

                    float a = Detail::clamp01( src[3] );
                    float r = mt::clampf( 0.f, src[0], a );
                    float g = mt::clampf( 0.f, src[1], a );
                    float b = mt::clampf( 0.f, src[2], a );

                    uint8_t r8 = (uint8_t)(r * 255.f + 0.5f);
                    uint8_t g8 = (uint8_t)(g * 255.f + 0.5f);
                    uint8_t b8 = (uint8_t)(b * 255.f + 0.5f);
                    uint8_t a8 = (uint8_t)(a * 255.f + 0.5f);

                    if( _rgba == true )
                    {
                        dst[0] = r8;
                        dst[1] = g8;
                        dst[2] = b8;
                        dst[3] = a8;
                    }
                    else
                    {
                        dst[0] = b8;
                        dst[1] = g8;
                        dst[2] = r8;
                        dst[3] = a8;
                    }
                }
            }
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
