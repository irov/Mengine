#	pragma once

#	include "Interface/TextInterface.h"

#	include "Core/ConstString.h"
#	include "Core/ColourValue.h"

#	include "Factory/Factorable.h"

namespace Menge
{
	class TextEntry
		: public TextEntryInterface
		, public Factorable
	{
	public:
		TextEntry();

	public:
		void initialize( const ConstString & _text, const ConstString & _font, const ColourValue & _colorFont, const ColourValue & _colorOutline, float _lineOffset, float _charOffset, size_t _params );

	public:
		const ConstString & getText() const override;

	public:
		const ConstString & getFont() const override;
		const ColourValue & getColorFont() const override;
		const ColourValue & getColorOutline() const override;
		float getLineOffset() const override;
		float getCharOffset() const override;

	public:
		size_t getFontParams() const override;

	protected:
		ConstString m_text;
		ConstString m_font;

		ColourValue m_colorFont;
		ColourValue m_colorOutline;

		float m_lineOffset;
		float m_charOffset;

		size_t m_params;
	};
}