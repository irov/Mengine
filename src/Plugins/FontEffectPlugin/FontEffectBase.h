#pragma once

#include "Interface/FontEffectInterface.h"
#include "Interface/FileGroupInterface.h"

#include "FontEffectPluginInterface.h"
#include "FontEffectRaster.h"

#include "Kernel/Vector.h"
#include "Kernel/Map.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FontEffectBase
        : public FontEffectInterface
    {
        DECLARE_FACTORABLE( FontEffectBase );

    public:
        FontEffectBase();
        ~FontEffectBase() override;

    public:
        void setContent( const ContentInterfacePtr & _content ) override;
        const ContentInterfacePtr & getContent() const override;

    public:
        void setEffectSample( uint32_t _effectSample ) override;
        uint32_t getEffectSample() const override;
        float getEffectSampleInv() const override;

    public:
        uint32_t getLayoutCount() const override;

    public:
        bool apply( uint32_t _width, uint32_t _rows, uint32_t _pitch, const void * _buffer, uint32_t _channel, int32_t _left, int32_t _top, uint32_t _height, const LambdaFontEffectProvider & _provider ) override;

    public:
        bool prefetch( const PrefetcherObserverInterfacePtr & _observer ) override;
        bool unfetch() override;

    public:
        bool isValid() const override;

    public:
        bool setupDesc( const FontEffectDesc & _desc );

    public:
        bool addLayer( uint32_t * const _layerIndex );
        bool setLayerEnabled( uint32_t _layerIndex, bool _enabled );
        bool setLayerOpacity( uint32_t _layerIndex, float _opacity );
        bool setLayerBlendMode( uint32_t _layerIndex, EFontEffectBlendMode _blendMode );
        bool setLayerKnockout( uint32_t _layerIndex, bool _knockout );
        bool setLayerMerge( uint32_t _layerIndex, bool _merge );

    public:
        bool addEffect( uint32_t _layerIndex, EFontEffectType _type, uint32_t * const _effectIndex );
        bool setEffectEnabled( uint32_t _layerIndex, uint32_t _effectIndex, bool _enabled );
        bool setEffectOpacity( uint32_t _layerIndex, uint32_t _effectIndex, float _opacity );
        bool setEffectBlendMode( uint32_t _layerIndex, uint32_t _effectIndex, EFontEffectBlendMode _blendMode );
        bool setEffectColor( uint32_t _layerIndex, uint32_t _effectIndex, const Color & _color );
        bool setEffectWidth( uint32_t _layerIndex, uint32_t _effectIndex, float _width );
        bool setEffectSharpness( uint32_t _layerIndex, uint32_t _effectIndex, float _sharpness );
        bool setEffectOutlinePosition( uint32_t _layerIndex, uint32_t _effectIndex, EFontEffectOutlinePosition _position );
        bool setEffectOffset( uint32_t _layerIndex, uint32_t _effectIndex, const mt::vec2f & _offset );
        bool setEffectBlur( uint32_t _layerIndex, uint32_t _effectIndex, float _blur );
        bool setEffectSpread( uint32_t _layerIndex, uint32_t _effectIndex, float _spread );
        bool setEffectDistance( uint32_t _layerIndex, uint32_t _effectIndex, float _distance );
        bool setEffectInvert( uint32_t _layerIndex, uint32_t _effectIndex, bool _invert );
        bool setEffectDepth( uint32_t _layerIndex, uint32_t _effectIndex, float _depth );
        bool setEffectSize( uint32_t _layerIndex, uint32_t _effectIndex, float _size );
        bool setEffectSoften( uint32_t _layerIndex, uint32_t _effectIndex, float _soften );
        bool setEffectAngle( uint32_t _layerIndex, uint32_t _effectIndex, float _angle );
        bool setEffectAltitude( uint32_t _layerIndex, uint32_t _effectIndex, float _altitude );
        bool setEffectHighlight( uint32_t _layerIndex, uint32_t _effectIndex, const Color & _color );
        bool setEffectShadow( uint32_t _layerIndex, uint32_t _effectIndex, const Color & _color );

    public:
        bool setEffectGradientEnabled( uint32_t _layerIndex, uint32_t _effectIndex, bool _enabled );
        bool setEffectGradientType( uint32_t _layerIndex, uint32_t _effectIndex, EFontEffectGradientType _type );
        bool setEffectGradientAngle( uint32_t _layerIndex, uint32_t _effectIndex, float _angle );
        bool setEffectGradientCenter( uint32_t _layerIndex, uint32_t _effectIndex, const mt::vec2f & _center );
        bool setEffectGradientScale( uint32_t _layerIndex, uint32_t _effectIndex, float _scale );
        bool setEffectGradientReverse( uint32_t _layerIndex, uint32_t _effectIndex, bool _reverse );
        bool setEffectGradientDither( uint32_t _layerIndex, uint32_t _effectIndex, bool _dither );

    public:
        bool setEffectPatternFilePath( uint32_t _layerIndex, uint32_t _effectIndex, const FilePath & _filePath );
        bool setEffectPatternCodecType( uint32_t _layerIndex, uint32_t _effectIndex, const ConstString & _codecType );
        bool setEffectPatternSpace( uint32_t _layerIndex, uint32_t _effectIndex, EFontEffectSpace _space );
        bool setEffectPatternTile( uint32_t _layerIndex, uint32_t _effectIndex, EFontEffectPatternTile _tile );
        bool setEffectPatternScale( uint32_t _layerIndex, uint32_t _effectIndex, float _scale );
        bool setEffectPatternOffset( uint32_t _layerIndex, uint32_t _effectIndex, const mt::vec2f & _offset );
        bool setEffectPatternAngle( uint32_t _layerIndex, uint32_t _effectIndex, float _angle );
        bool setEffectGradientSpace( uint32_t _layerIndex, uint32_t _effectIndex, EFontEffectSpace _space );
        bool addEffectGradientStop( uint32_t _layerIndex, uint32_t _effectIndex, float _t, const Color & _color );

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        virtual bool _prefetch( const PrefetcherObserverInterfacePtr & _observer );
        virtual bool _unfetch();

    protected:
        void applySampleHint_( uint32_t _sample );

    protected:
        bool setupEffectCommon_( uint32_t _layerIndex, uint32_t _effectIndex, const FontEffectStyleDesc & _effect );
        bool setupEffectBevel_( uint32_t _layerIndex, uint32_t _effectIndex, const FontEffectStyleDesc & _effect );
        bool setupEffectGradient_( uint32_t _layerIndex, uint32_t _effectIndex, const FontEffectGradientDesc & _gradient );
        bool setupEffectPattern_( uint32_t _layerIndex, uint32_t _effectIndex, const FontEffectPatternDesc & _pattern );

    protected:
        void composeLayer_( const FontEffectLayerDesc & _layer, uint32_t _width, uint32_t _rows, uint32_t _channel, int32_t _top, uint32_t _height );
        bool flushLayout_( uint32_t _layoutIndex, int32_t _left, int32_t _top, const LambdaFontEffectProvider & _provider );
        void makeGradientContext_( const FontEffectGradientDesc & _gradient, uint32_t _width, uint32_t _rows, int32_t _top, uint32_t _height, FontEffectGradientContext * const _context ) const;
        void ensureSignedDistance_();
        bool compilePatterns_();
        bool compilePattern_( const FilePath & _filePath, const ConstString & _codecType );
        FileGroupInterfacePtr getPatternFileGroup_() const;
        void makePatternContext_( const FontEffectPatternDesc & _pattern, const FontEffectPatternImage & _image, uint32_t _width, uint32_t _rows, int32_t _top, uint32_t _height, FontEffectPatternContext * const _context ) const;

    protected:
        FontEffectLayerDesc * getLayer_( uint32_t _layerIndex );
        FontEffectStyleDesc * getEffect_( uint32_t _layerIndex, uint32_t _effectIndex );

    protected:
        ContentInterfacePtr m_content;

        FontEffectDesc m_desc;

        uint32_t m_effectSample;
        float m_effectSampleInv;
        bool m_effectSampleExplicit;

        uint32_t m_layoutCount;
        uint32_t m_padding;
        bool m_defaultFill;

        FontEffectScratch m_scratch;

        //////////////////////////////////////////////////////////////////////////
        typedef Map<FilePath, FontEffectPatternImage> MapFontEffectPatternImages;
        //////////////////////////////////////////////////////////////////////////
        MapFontEffectPatternImages m_patterns;

        //////////////////////////////////////////////////////////////////////////
        typedef Vector<uint8_t> VectorFontEffectExportBuffer;
        //////////////////////////////////////////////////////////////////////////
        VectorFontEffectExportBuffer m_exportBuffer;

        bool m_sdfValid;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FontEffectBase, FontEffectInterface> FontEffectBasePtr;
    //////////////////////////////////////////////////////////////////////////
}
