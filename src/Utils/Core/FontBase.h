#	pragma once

#	include "Interface/TextInterface.h"

#   include "Core/ServantBase.h"

namespace Menge
{
	class FontBase
		: public ServantBase<TextFontInterface>
	{
	public:
		FontBase();
		~FontBase();

	public:
		void setName( const ConstString & _name ) override;
		const ConstString & getName() const override;

	public:
		void setColourFont( const ColourValue & _colour ) override;
		const ColourValue & getColorFont() const override;

		void setColourOutline( const ColourValue & _colour ) override;
		const ColourValue & getColorOutline() const override;

		void setLineOffset( float _lineOffset ) override;
		float getLineOffset() const override;

		void setCharOffset( float _charOffset ) override;
		float getCharOffset() const override;

	public:
		uint32_t getFontParams() const override;

	public:
		U32String prepareText( const String & _text ) override;

	protected:
		bool initializeBase_( const IniUtil::IniStore & _ini );
		
	protected:
		virtual bool _prepareGlyph( uint32_t _code ) = 0;

	protected:
		ConstString m_name;

		float m_height;

		uint32_t m_params;

		ColourValue m_colourFont;
		ColourValue m_colourOutline;

		float m_lineOffset;
		float m_charOffset;
	};
}
