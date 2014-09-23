#	pragma once

#	include "Interface/TextInterface.h"

#	include "Core/ConstString.h"
#	include "Core/ColourValue.h"

#	include "stdex/intrusive_splay_tree.h"

namespace Menge
{
	class TextEntry
		: public TextEntryInterface
		, public stdex::intrusive_splay_node<TextEntry>
	{
	public:
		typedef ConstString key_type;	
		typedef ConstString::less_type less_type;

		struct key_getter_type
		{
			const ConstString & operator()( const TextEntry * _node ) const
			{
				return _node->m_key;
			}
		};

	public:
		TextEntry();

	public:
		void initialize( const ConstString & _key, const ConstString & _text, const ConstString & _font, const ColourValue & _colorFont, const ColourValue & _colorOutline, float _lineOffset, float _charOffset, float _maxLength, size_t _params );

	public:
		const ConstString & getKey() const override;
		const ConstString & getText() const override;

	public:
		const ConstString & getFont() const override;
		const ColourValue & getColorFont() const override;
		const ColourValue & getColorOutline() const override;
		float getLineOffset() const override;
		float getCharOffset() const override;
		float getMaxLength() const override;

	public:
		uint32_t getFontParams() const override;

	protected:
		ConstString m_key;
		ConstString m_text;

		ConstString m_font;

		ColourValue m_colorFont;
		ColourValue m_colorOutline;

		float m_lineOffset;
		float m_charOffset;
		float m_maxLength;

		uint32_t m_params;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<TextEntry> TextEntryPtr;
}