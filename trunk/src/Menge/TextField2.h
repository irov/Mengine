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
		enum VerticalAlign
		{
			VA_TOP = 0,
			VA_CENTER,
			VA_BOTTOM
		};
		enum HorizontalAlign
		{
			HA_LEFT = 0,
			HA_CENTER,
			HA_RIGHT
		};

	public:
		void setFieldSize( const mt::vec2f& _fieldSize );
		const mt::vec2f& getFieldSize() const;
		void setVerticalAlign( VerticalAlign _align );
		VerticalAlign getVerticalAlign() const;
		void setHorizontalAlign( HorizontalAlign _align );
		HorizontalAlign getHorizontalAlign() const;
		void setText( const String& _text );
		const String& getText() const;
		void setTextByKey( const String& _key );
		void setFont( const String& _font );
		const String& getFont() const;
		void setCharOffset( float _offset );
		float getCharOffset() const;
		void setLineOffset( float _offset );
		float getLineOffset() const;

	public:
		void loader( XmlElement * _xml ) override;
		void _render( Camera2D * _camera ) override;

	protected:
		bool _compile() override;
		void _release() override;

	private:
		void updateVertices_();

	private:
		mt::vec2f m_fieldSize;
		VerticalAlign m_verticalAlign;
		HorizontalAlign m_horizontalAlign;
		String m_text;
		String m_font;
		ResourceFont* m_resourceFont;
		float m_charOffset;
		float m_lineOffset;

		TVertex2DVector m_vertices;
		bool m_invalidateVertices;
	};
}	// namespace Menge
