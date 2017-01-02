#	pragma once

#	include "Interface/TextInterface.h"

#	include "Kernel/Resource.h"
#	include "Kernel/Reference.h"

#	include "TextGlyph.h"

namespace Menge
{
	class TextFont
		: public TextFontInterface
		, public Resource
		, public Reference
	{
	public:
		TextFont();
		~TextFont();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider );

    public:
		void setName( const ConstString & _name );
		
		void setGlyph( const TextGlyphPtr & _glyph );
		void setTexturePath( const ConstString & _category, const FilePath & _pathFontImage, const FilePath & _pathOutlineImage );
		void setColourFont( const ColourValue & _colour );
		void setColourOutline( const ColourValue & _colour );
		void setLineOffset( float _lineOffset );
		void setCharOffset( float _charOffset );

	public:
		const ConstString & getName() const override;

	public:
		bool getGlyph( GlyphCode _char, GlyphCode _next, Glyph * _glyph ) const override;
		bool hasGlyph( GlyphCode _char ) const override;

	public:
		float getFontHeight() const override;

	public:
		uint32_t getFontParams() const override;

		const ColourValue & getColorFont() const override;
		const ColourValue & getColorOutline() const override;

		float getLineOffset() const override;
		float getCharOffset() const override;

	public:
		const RenderTextureInterfacePtr & getTextureFont() const override;
		const RenderTextureInterfacePtr & getTextureOutline() const override;


	protected:
		bool _compile() override;
		void _release() override;

	protected:
		bool _incrementZero() override;
		void _decrementZero() override;

	protected:
		ServiceProviderInterface * m_serviceProvider;

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