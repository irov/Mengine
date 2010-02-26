/*
 *	TextField2.cpp
 *
 *	Created by _Berserk_ on 25.2.2010
 *	Copyright 2010 Menge. All rights reserved.
 *
 */

#include "TextField2.h"
#include "ObjectImplement.h"

#include "ResourceFont.h"
#include "ResourceManager.h"
#include "XmlEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FACTORABLE_IMPLEMENT(TextField2);
	//////////////////////////////////////////////////////////////////////////
	TextField2::TextField2()
		: m_fieldSize( 0.0f, 0.0f )
		, m_verticalAlign( VA_TOP )
		, m_horizontalAlign( HA_LEFT )
		, m_resourceFont( NULL )
		, m_invalidateVertices( true )
		, m_charOffset( 0.0f )
		, m_lineOffset( 0.0f )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	TextField2::~TextField2()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField2::setFieldSize( const mt::vec2f& _fieldSize )
	{
		m_fieldSize = _fieldSize;
		m_invalidateVertices = true;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f& TextField2::getFieldSize() const
	{
		return m_fieldSize;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField2::setVerticalAlign( VerticalAlign _align )
	{
		m_verticalAlign = _align;
	}
	//////////////////////////////////////////////////////////////////////////
	TextField2::VerticalAlign TextField2::getVerticalAlign() const
	{
		return m_verticalAlign;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField2::setHorizontalAlign( HorizontalAlign _align )
	{
		m_horizontalAlign = _align;
	}
	//////////////////////////////////////////////////////////////////////////
	TextField2::HorizontalAlign TextField2::getHorizontalAlign() const
	{
		return m_horizontalAlign;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField2::setText( const String& _text )
	{
		m_text = _text;
		m_invalidateVertices = true;
	}
	//////////////////////////////////////////////////////////////////////////
	const String& TextField2::getText() const
	{
		return m_text;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField2::setTextByKey( const String& _key )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField2::setFont( const String& _font )
	{
		m_font = _font;

		m_invalidateVertices = true;
	}
	//////////////////////////////////////////////////////////////////////////
	const String& TextField2::getFont() const
	{
		return m_font;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField2::setCharOffset( float _offset )
	{
		m_charOffset = _offset;
		m_invalidateVertices = true;
	}
	//////////////////////////////////////////////////////////////////////////
	float TextField2::getCharOffset() const
	{
		return m_charOffset;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField2::setLineOffset( float _offset )
	{
		m_lineOffset = _offset;
		m_invalidateVertices = true;
	}
	//////////////////////////////////////////////////////////////////////////
	float TextField2::getLineOffset() const
	{
		return m_lineOffset;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField2::loader( XmlElement * _xml )
	{
		Node::loader(_xml);

		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Font", "Name", &TextField2::setFont );
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Text", "Value", &TextField2::setTextByKey );
			XML_CASE_ATTRIBUTE_NODE_METHOD( "FieldSize", "Value", &TextField2::setFieldSize  );
			XML_CASE_ATTRIBUTE_NODE_METHODT( "VerticalAlign", "Value", &TextField2::setVerticalAlign, int );
			XML_CASE_ATTRIBUTE_NODE_METHODT( "HorizontalAlign", "Value", &TextField2::setHorizontalAlign, int );
			XML_CASE_ATTRIBUTE_NODE_METHOD( "CharOffset", "Value", &TextField2::setCharOffset );
			XML_CASE_ATTRIBUTE_NODE_METHOD( "LineOffset", "Value", &TextField2::setLineOffset );
		}

	}
	//////////////////////////////////////////////////////////////////////////
	void TextField2::_render( Camera2D * _camera )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextField2::_compile()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField2::_release()
	{
		m_vertices.clear();

		if( m_resourceFont != NULL )
		{
			ResourceManager::hostage()
				->releaseResource( m_resourceFont );
			m_resourceFont = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField2::updateVertices_()
	{
		m_invalidateVertices = false;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
