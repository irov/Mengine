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

    protected:
		bool initialize( const ConstString & _category, const IniUtil::IniStore & _ini ) override;

	protected:
		bool _compile() override;
		void _release() override;

    public:
		void setGlyph( const BitmapGlyphPtr & _glyph );

    protected:
		bool hasGlyph( GlyphCode _code ) const override;
		bool getGlyph( GlyphCode _code, GlyphCode _next, Glyph * _glyph ) const override;

    protected:
        float getFontAscent() const override;
        float getFontDescent() const override;
        float getFontHeight() const override;
        float getFontSpacing() const override;

    protected:
        bool getFontPremultiply() const override;

    protected:
        bool _validateGlyphes( const U32String & _codes ) const override;
		bool _prepareGlyph( GlyphCode _code ) override;

	protected:
		BitmapGlyphPtr m_glyph;
		
		ConstString m_category;
		FilePath m_pathFontImage;
		FilePath m_pathOutlineImage;

        float m_height;

		RenderTextureInterfacePtr m_textureFont;
		RenderTextureInterfacePtr m_textureOutline;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<BitmapFont> BitmapFontPtr;
}