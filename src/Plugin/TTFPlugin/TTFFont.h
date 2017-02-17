#	pragma once

#	include "TTFServiceInterface.h"

#	include "Interface/MemoryInterface.h"
#	include "Interface/RenderSystemInterface.h"

#	include "Config/String.h"

#	include "Factory/FactorablePtr.h"

#	include "Kernel/Servant.h"

#	include "ft2build.h"
#	include "freetype/freetype.h"
#	include "freetype/ftglyph.h"

#	include "Math/uv4.h"

#	include "stdex/stl_map.h"

namespace Menge
{
	struct TTFGlyph
	{
		WChar ch;

		float dx;
		float dy;
		float ax;
		float ay;

		mt::uv4f uv;
		RenderTextureInterfacePtr texture;
	};

#	define MENGINE_TTF_FONT_GLYPH_HASH_SIZE 32

	class TTFFont
		: public TTFFontInterface
	{
	public:
		TTFFont();
		~TTFFont();

	public:
		bool initialize( FT_Library _library, const MemoryInterfacePtr & _memory );

	public:
		bool prepareText( const String & _text );

	protected:
		bool prepareGlyph_( WChar _ch );
		const TTFGlyph * getGlyph_( WChar _ch ) const;

	protected:
		FT_Library m_library;
		FT_Face m_face;

		float m_ascender;
		float m_advance;
		
		typedef stdex::vector<TTFGlyph> TMapTTFGlyphs;
		TMapTTFGlyphs m_glyphsHash[MENGINE_TTF_FONT_GLYPH_HASH_SIZE];
	};

	typedef stdex::intrusive_ptr<TTFFont> TTFFontPtr;
}