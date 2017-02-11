#	pragma once

#	include "Interface/MemoryInterface.h"
#	include "Interface/RenderSystemInterface.h"

#	include "Config/String.h"

#	include "Factory/FactorablePtr.h"

#	include "Kernel/Servant.h"

#	include "ft2build.h"
#	include "freetype/freetype.h"
#	include "freetype/ftglyph.h"

#	include "Math/uv4.h"

namespace Menge
{
	struct TTFGlyph
	{
		float dx;
		float dy;
		float ax;
		float ay;

		mt::uv4f uv;
		RenderTextureInterfacePtr texture;
	};

	class TTFFont
		: public FactorablePtr
		, public Servant
	{
	public:
		TTFFont();
		~TTFFont();

	public:
		bool initialize( FT_Library _library, const MemoryInterfacePtr & _memory );

	public:
		bool prepareText( const String & _text );

	protected:
		FT_Library m_library;
		FT_Face m_face;

		//stdex::map<wchar_t, TTFGlyph>;
	};
}