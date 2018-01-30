#	pragma once

#	include "TTFServiceInterface.h"

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/MemoryInterface.h"
#	include "Interface/TextInterface.h"

#	include "Config/String.h"

#	include "Factory/FactorablePtr.h"

#	include "Kernel/Servant.h"

#   include "Core/FontBase.h"

#	include "ft2build.h"
#	include "freetype/freetype.h"
#	include "freetype/ftglyph.h"

#	include "Math/uv4.h"

#	include "stdex/stl_map.h"

namespace Menge
{
#	define MENGINE_TTF_FONT_GLYPH_HASH_SIZE 37

	class TTFFont
		: public FontBase
	{
	public:
		TTFFont();
		~TTFFont();

	public:
		void setFTLibrary( FT_Library _library );

	public:
		bool initialize( const ConstString & _category, const IniUtil::IniStore & _ini ) override;

	protected:
		bool _compile() override;
		void _release() override;

	public:
		bool hasGlyph( GlyphCode _code ) const override;
		bool getGlyph( GlyphCode _code, GlyphCode _next, Glyph * _glyph ) const override;

    protected:
        float getFontAscent() const override;
        float getFontDescent() const override;
        float getFontHeight() const override;
        float getFontSpacing() const override;

	protected:
        bool _validateGlyph( GlyphCode _code ) const override;
		bool _prepareGlyph( GlyphCode _code ) override;

	protected:
		MemoryInterfacePtr m_memory;

		FT_Library m_ftlibrary;
		FT_Face m_face;

		ConstString m_category;
		FilePath m_ttfPath;
        
		uint32_t m_ttfDPI;

        float m_ttfAscender;
        float m_ttfDescender;
        float m_ttfHeight;
        float m_ttfSpacing;
		
		typedef stdex::vector<TTFGlyph> TVectorTTFGlyphs;
		TVectorTTFGlyphs m_glyphsHash[MENGINE_TTF_FONT_GLYPH_HASH_SIZE];
	};

	typedef stdex::intrusive_ptr<TTFFont> TTFFontPtr;
}