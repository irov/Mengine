#	pragma once

#	include "TTFServiceInterface.h"

#	include "Interface/RenderSystemInterface.h"
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
#	define MENGINE_TTF_FONT_GLYPH_HASH_SIZE 32

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
		bool hasGlyph( GlyphCode _char ) const override;
		bool getGlyph( GlyphCode _char, GlyphCode _next, Glyph * _glyph ) const override;

	protected:
		float getFontHeight() const override;

	protected:
		bool _prepareGlyph( GlyphCode _ch ) override;

	protected:
		FT_Library m_ftlibrary;
		FT_Face m_face;

		ConstString m_category;
		FilePath m_ttfPath;

		uint32_t m_fontDPI;

		float m_ascender;
		
		typedef stdex::vector<TTFGlyph> TVectorTTFGlyphs;
		TVectorTTFGlyphs m_glyphsHash[MENGINE_TTF_FONT_GLYPH_HASH_SIZE];
	};

	typedef stdex::intrusive_ptr<TTFFont> TTFFontPtr;
}