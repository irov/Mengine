#	include "TextEntry.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TextEntry::TextEntry()
		: m_lineOffset(0.f)
		, m_charOffset(0.f)
		, m_maxLength(0.f)
		, m_params(EFP_NONE)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void TextEntry::initialize( const ConstString & _key, const ConstString & _text, const ConstString & _font, const ColourValue & _colorFont, const ColourValue & _colorOutline, float _lineOffset, float _charOffset, float _maxLength, size_t _params )
	{
		m_key = _key;
		m_text = _text;

		m_font = _font;

		m_colorFont = _colorFont;
		m_colorOutline = _colorOutline;

		m_lineOffset = _lineOffset;
		m_charOffset = _charOffset;
		m_maxLength = _maxLength;

		m_params = _params;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & TextEntry::getKey() const
	{
		return m_key;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & TextEntry::getText() const
	{
		return m_text;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & TextEntry::getFont() const
	{
		return m_font;
	}
	//////////////////////////////////////////////////////////////////////////
	const ColourValue & TextEntry::getColorFont() const
	{
		return m_colorFont;
	}
	//////////////////////////////////////////////////////////////////////////
	const ColourValue & TextEntry::getColorOutline() const
	{
		return m_colorOutline;
	}
	//////////////////////////////////////////////////////////////////////////
	float TextEntry::getLineOffset() const
	{
		return m_lineOffset;
	}
	//////////////////////////////////////////////////////////////////////////
	float TextEntry::getCharOffset() const
	{
		return m_charOffset;
	}
	//////////////////////////////////////////////////////////////////////////
	float TextEntry::getMaxLength() const
	{
		return m_maxLength;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t TextEntry::getFontParams() const
	{
		return m_params;
	}
}