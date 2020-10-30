#pragma once

#include "TTFInterface.h"
#include "TTFDataInterface.h"
#include "FEDataInterface.h"

#include "Interface/MemoryInterface.h"
#include "Interface/TextFontInterface.h"

#include "Kernel/Observable.h"
#include "Kernel/String.h"
#include "Kernel/Factorable.h"
#include "Kernel/FontBase.h"
#include "Kernel/Map.h"

#include "ft2build.h"
#include "freetype/freetype.h"
#include "freetype/ftglyph.h"

#include "math/uv4.h"

#include "fe/fe.h"

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_TTF_FONT_GLYPH_HASH_SIZE
#define MENGINE_TTF_FONT_GLYPH_HASH_SIZE 37
#endif
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    class TTFFont
        : public FontBase
        , public UnknownTTFFontInterface
        , public Observable
    {
        DECLARE_UNKNOWABLE();

    public:
        TTFFont();
        ~TTFFont() override;

    public:
        void setFTLibrary( FT_Library _library );

    public:
        void setSystem( bool _system ) override;
        bool getSystem() const override;

        void setTTFPath( const FilePath & _ttfPath ) override;
        const FilePath & getTTFPath() const override;

        void setTTFFileGroup( const FileGroupInterfacePtr & _fileGroup ) override;
        const FileGroupInterfacePtr & getTTFFileGroup() const override;

        void setTTFFEPath( const FilePath & _ttfPath ) override;
        const FilePath & getTTFFEPath() const override;

        void setTTFFEName( const ConstString & _ttfPath ) override;
        const ConstString & getTTFFEName() const override;


        void setTTFFEFileGroup( const FileGroupInterfacePtr & _fileGroup ) override;
        const FileGroupInterfacePtr & getTTFFEFileGroup() const override;

        void setFESample( uint32_t _FESample ) override;
        uint32_t getFESample() const override;

        void setFECustomString( const String & _FECustomString ) override;
        const String & getFECustomString() const override;

    public:
        bool initialize() override;
        void finalize() override;

    protected:
        bool isValid() override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        bool _prefetch( const PrefetcherObserverInterfacePtr & _observer ) override;
        bool _unfetch() override;

    public:
        bool hasGlyph( GlyphCode _code ) const override;
        bool getGlyph( uint32_t _layout, GlyphCode _code, GlyphCode _next, Glyph * const _glyph ) const override;

    protected:
        uint32_t getLayoutCount() const override;

    protected:
        float getFontAscent() const override;
        float getFontDescent() const override;
        float getFontHeight() const override;
        float getFontBearingYA() const override;
        float getFontSpacing() const override;

    protected:
        bool getFontPremultiply() const override;

    protected:
        bool _validateGlyphes( const U32String & _codes ) const override;
        bool _prepareGlyph( GlyphCode _code, const DocumentPtr & _doc ) override;

    protected:
        bool loadFaceGlyph_( GlyphCode _code, FT_Face * const _face );

    protected:
        void notifyRenderDeviceLostPrepare();

    protected:
        void clearGlyphs_();

    protected:
        FT_Library m_ftlibrary;
        TTFDataInterfacePtr m_dataTTF;

        FileGroupInterfacePtr m_ttfFileGroup;
        FilePath m_ttfPath;

        bool m_system;

        uint32_t m_FESample;

        String m_FECustomString;

        float m_ttfAscender;
        float m_ttfDescender;
        float m_ttfHeight;
        float m_ttfBearingYA;
        float m_ttfSpacing;

        uint32_t m_ttfLayoutCount;

        FileGroupInterfacePtr m_ttfFEFileGroup;
        FilePath m_ttfFEPath;

        ConstString m_ttfFEName;

        FEDataInterfacePtr m_dataFE;
        fe_effect * m_ttfFEEffect;
        const fe_node * m_ttfEffectNodes[FE_MAX_PINS] = {nullptr};

        struct TTFGlyphQuad
        {
            mt::vec2f offset;
            mt::vec2f size;

            mt::uv4f uv;
            RenderTextureInterfacePtr texture;
        };

        struct TTFGlyph
        {
            uint32_t ch;

            float advance;

            TTFGlyphQuad quads[4];
        };

        typedef Vector<TTFGlyph> VectorTTFGlyphs;
        VectorTTFGlyphs m_glyphsHash[MENGINE_TTF_FONT_GLYPH_HASH_SIZE];
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TTFFont> TTFFontPtr;
    //////////////////////////////////////////////////////////////////////////
}