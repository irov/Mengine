#	pragma once

#	include "Interface/TextInterface.h"

#	include "Kernel/Resource.h"
#	include "Kernel/Reference.h"

#   include "Core/FontBase.h"

#	include "TextGlyph.h"

namespace Menge
{
	class TextFont
		: public FontBase
	{
	public:
		TextFont();
		~TextFont();

    public:
		void setGlyph( const TextGlyphPtr & _glyph );
		void setTexturePath( const ConstString & _category, const FilePath & _pathFontImage, const FilePath & _pathOutlineImage );

	public:
		bool hasGlyph( GlyphCode _char ) const override;
		bool getGlyph( GlyphCode _char, GlyphCode _next, Glyph * _glyph ) const override;

	public:
		float getFontHeight() const override;

	public:
		const RenderTextureInterfacePtr & getTextureFont() const override;
		const RenderTextureInterfacePtr & getTextureOutline() const override;

	protected:
		TextGlyphPtr m_glyph;

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
	typedef stdex::intrusive_ptr<TextFont> TextFontPtr;
	//////////////////////////////////////////////////////////////////////////
	inline const RenderTextureInterfacePtr & TextFont::getTextureFont() const
	{
		return m_textureFont;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const RenderTextureInterfacePtr & TextFont::getTextureOutline() const
	{
		return m_textureOutline;
	}
}