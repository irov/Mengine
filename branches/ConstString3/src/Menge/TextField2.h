/*
 *	TextField2.h
 *
 *	Created by _Berserk_ on 25.2.2010
 *	Copyright 2010 Menge. All rights reserved.
 *
 */

#pragma  once

#	include "Node.h"

namespace Menge
{
	class ResourceFont;

	class TextField2
		: public Node
	{
	public:
		TextField2();
		~TextField2();
	
	public:
		enum EVerticalAlign
		{
			VA_TOP = 0,
			VA_CENTER,
			VA_BOTTOM
		};

		enum EHorizontalAlign
		{
			HA_LEFT = 0,
			HA_CENTER,
			HA_RIGHT
		};

	public:
		void setFieldSize( const mt::vec2f& _fieldSize );
		const mt::vec2f& getFieldSize() const;

		void setVerticalAlign( EVerticalAlign _align );
		EVerticalAlign getVerticalAlign() const;

		void setHorizontalAlign( EHorizontalAlign _align );
		EHorizontalAlign getHorizontalAlign() const;

		void setText( const String& _text );
		const String& getText() const;

		void setTextByKey( const ConstString& _key );
		void setFont( const ConstString& _font );

		const ConstString& getFont() const;
		void setCharOffset( float _offset );
		float getCharOffset() const;
		void setLineOffset( float _offset );
		float getLineOffset() const;

	protected:
		bool _compile() override;
		void _release() override;
		void _render( Camera2D * _camera ) override;

	private:
		void updateVertices_();

	private:
		mt::vec2f m_fieldSize;
		EVerticalAlign m_verticalAlign;
		EHorizontalAlign m_horizontalAlign;
		String m_text;
		ConstString m_font;
		ResourceFont* m_resourceFont;
		float m_charOffset;
		float m_lineOffset;

		TVectorVertex2D m_vertices;
		bool m_invalidateVertices;
	};
}	// namespace Menge
