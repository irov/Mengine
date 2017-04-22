#	pragma once

#	include "Interface/TextInterface.h"

#	include "Kernel/Resource.h"
#	include "Kernel/Reference.h"

#   include "Core/FontBase.h"

#	include "BitmapGlyph.h"

namespace Menge
{
	class BitmapFont
		: public FontBase
	{
	public:
		BitmapFont();
		~BitmapFont();

	public:
		bool initialize( const ConstString & _category, const IniUtil::IniStore & _ini ) override;

	protected:
		bool _compile() override;
		void _release() override;

    public:
		void setGlyph( const BitmapGlyphPtr & _glyph );

	public:
		bool hasGlyph( GlyphCode _char ) const override;
		bool getGlyph( GlyphCode _char, GlyphCode _next, Glyph * _glyph ) const override;

	public:
		float getFontHeight() const override;

	public:
		bool _prepareGlyph( GlyphCode _code ) override;

	protected:
		BitmapGlyphPtr m_glyph;
		
		ConstString m_category;
		FilePath m_pathFontImage;
		FilePath m_pathOutlineImage;

		RenderTextureInterfacePtr m_textureFont;
		RenderTextureInterfacePtr m_textureOutline;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<BitmapFont> BitmapFontPtr;
}