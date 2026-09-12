#pragma once

#include "Kernel/Color.h"
#include "Kernel/Vector.h"

#include "FontEffectDesc.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    struct FontEffectPlane
    {
        uint32_t width = 0;
        uint32_t height = 0;
        float * data = nullptr;
    };
    //////////////////////////////////////////////////////////////////////////
    struct FontEffectImage
    {
        uint32_t width = 0;
        uint32_t height = 0;
        float * data = nullptr;
    };
    //////////////////////////////////////////////////////////////////////////
    struct FontEffectRect
    {
        uint32_t x = 0;
        uint32_t y = 0;
        uint32_t width = 0;
        uint32_t height = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    struct FontEffectGradientContext
    {
        float centerX = 0.f;
        float centerY = 0.f;
        float dirX = 0.f;
        float dirY = 1.f;
        float angle = 0.f;
        float extentInv = 1.f;
        float radiusInv = 1.f;
    };
    //////////////////////////////////////////////////////////////////////////
    class FontEffectScratch
    {
    public:
        FontEffectScratch();
        ~FontEffectScratch();

    public:
        void setup( uint32_t _width, uint32_t _height );
        void clear();

    public:
        FontEffectPlane getPlane( uint32_t _index );
        FontEffectImage getImage( uint32_t _index );

    public:
        float * getLine( uint32_t _index );

    public:
        static const uint32_t MAX_PLANES = 8;
        static const uint32_t MAX_IMAGES = 3;
        static const uint32_t MAX_LINES = 4;

    protected:
        uint32_t m_width;
        uint32_t m_height;

        //////////////////////////////////////////////////////////////////////////
        typedef Vector<float> VectorFontEffectPlaneBuffer;
        //////////////////////////////////////////////////////////////////////////
        VectorFontEffectPlaneBuffer m_planes[MAX_PLANES];
        VectorFontEffectPlaneBuffer m_images[MAX_IMAGES];
        VectorFontEffectPlaneBuffer m_lines[MAX_LINES];
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        void fontEffectPlaneClear( const FontEffectPlane & _plane );
        void fontEffectPlaneCopy( const FontEffectPlane & _src, const FontEffectPlane & _dst );
        void fontEffectImageClear( const FontEffectImage & _image );
        void fontEffectImageCopy( const FontEffectImage & _src, const FontEffectImage & _dst );

        void fontEffectImportA8( const void * _buffer, uint32_t _width, uint32_t _rows, uint32_t _pitch, uint32_t _padding, const FontEffectPlane & _alpha );
        void fontEffectImportBGRA( const void * _buffer, uint32_t _width, uint32_t _rows, uint32_t _pitch, uint32_t _padding, const FontEffectPlane & _alpha, const FontEffectImage & _image );

        void fontEffectSignedDistance( const FontEffectPlane & _alpha, const FontEffectPlane & _sdf, const FontEffectPlane & _tmp0, const FontEffectPlane & _tmp1, float * _lineF, float * _lineD, float * _lineZ, float * _lineV );

        void fontEffectCoverage( const FontEffectPlane & _sdf, float _width, float _sharpness, const FontEffectPlane & _out );
        void fontEffectCoverageBand( const FontEffectPlane & _sdf, float _inner, float _outer, float _sharpness, const FontEffectPlane & _out );
        void fontEffectInvert( const FontEffectPlane & _src, const FontEffectPlane & _dst );
        void fontEffectMultiply( const FontEffectPlane & _plane, const FontEffectPlane & _mask );
        void fontEffectOffset( const FontEffectPlane & _src, int32_t _dx, int32_t _dy, const FontEffectPlane & _dst );

        void fontEffectBlurPlane( const FontEffectPlane & _plane, float _sigma, const FontEffectPlane & _tmp );
        void fontEffectBlurImage( const FontEffectImage & _image, float _sigma, const FontEffectPlane & _tmp0, const FontEffectPlane & _tmp1 );

        void fontEffectColorize( const FontEffectPlane & _coverage, const Color & _color, float _opacity, const FontEffectImage & _out );
        void fontEffectColorizeGradient( const FontEffectPlane & _coverage, const FontEffectPlane & _sdf, const FontEffectGradientDesc & _gradient, const FontEffectGradientContext & _context, float _opacity, const FontEffectImage & _out );
        void fontEffectTintImage( const FontEffectImage & _src, const Color & _color, float _opacity, const FontEffectImage & _out );
        void fontEffectTintImageGradient( const FontEffectImage & _src, const FontEffectPlane & _sdf, const FontEffectGradientDesc & _gradient, const FontEffectGradientContext & _context, float _opacity, const FontEffectImage & _out );

        void fontEffectSatin( const FontEffectPlane & _alpha, const FontEffectStyleDesc & _effect, float _sample, const FontEffectPlane & _work0, const FontEffectPlane & _work1, const FontEffectPlane & _tmp0, const FontEffectImage & _out );

        void fontEffectBevel( const FontEffectPlane & _sdf, const FontEffectPlane & _alpha, const FontEffectStyleDesc & _effect, float _sample, const FontEffectPlane & _tmp0, const FontEffectPlane & _tmp1, const FontEffectImage & _out );

        void fontEffectCompositeOver( const FontEffectImage & _dst, const FontEffectImage & _src );
        void fontEffectImageOpacity( const FontEffectImage & _image, float _opacity );

        bool fontEffectTightCrop( const FontEffectImage & _image, FontEffectRect * const _rect );
        void fontEffectExport8( const FontEffectImage & _image, const FontEffectRect & _rect, bool _rgba, uint8_t * const _buffer );
    }
}
