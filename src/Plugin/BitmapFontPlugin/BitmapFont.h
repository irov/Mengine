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
		bool initialize( const IniUtil::IniStore & _ini ) override;

    public:
		void setGlyph( const BitmapGlyphPtr & _glyph );

	public:
		bool hasGlyph( GlyphCode _char ) const override;
		bool getGlyph( GlyphCode _char, GlyphCode _next, Glyph * _glyph ) const override;

	public:
		float getFontHeight() const override;

	public:
		bool _prepareGlyph( uint32_t _code ) override;

	public:
		const RenderTextureInterfacePtr & getTextureFont() const override;
		const RenderTextureInterfacePtr & getTextureOutline() const override;

	protected:
		BitmapGlyphPtr m_glyph;

		ConstString m_name;
		float m_height;
		
		uint32_t m_params;

		ColourValue m_colourFont;
		ColourValue m_colourOutline;

		float m_lineOffset;
		float m_charOffset;

		ConstString m_category;
		FilePath m_pathFontImage;
		FilePath m_pathOutlineImage;

		RenderTextureInterfacePtr m_textureFont;
		RenderTextureInterfacePtr m_textureOutline;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<BitmapFont> BitmapFontPtr;
	//////////////////////////////////////////////////////////////////////////
	inline const RenderTextureInterfacePtr & BitmapFont::getTextureFont() const
	{
		return m_textureFont;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const RenderTextureInterfacePtr & BitmapFont::getTextureOutline() const
	{
		return m_textureOutline;
	}
}