/*
 *	TextField2.cpp
 *
 *	Created by _Berserk_ on 25.2.2010
 *	Copyright 2010 Menge. All rights reserved.
 *
 */

#include "TextField2.h"

#include "ResourceFont.h"
#include "ResourceManager.h"
#include "BinParser.h"

namespace Menge
{
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
	void TextField2::setVerticalAlign( EVerticalAlign _align )
	{
		m_verticalAlign = _align;
	}
	//////////////////////////////////////////////////////////////////////////
	TextField2::EVerticalAlign TextField2::getVerticalAlign() const
	{
		return m_verticalAlign;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField2::setHorizontalAlign( EHorizontalAlign _align )
	{
		m_horizontalAlign = _align;
	}
	//////////////////////////////////////////////////////////////////////////
	TextField2::EHorizontalAlign TextField2::getHorizontalAlign() const
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
	void TextField2::setTextByKey( const ConstString& _key )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField2::setFont( const ConstString& _font )
	{
		m_font = _font;

		m_invalidateVertices = true;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString& TextField2::getFont() const
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
	void TextField2::loader( BinParser * _parser )
	{
		Node::loader(_parser);

		BIN_SWITCH_ID(_parser)
		{
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Font_Name, &TextField2::setFont );
			//BIN_CASE_ATTRIBUTE_METHOD( Protocol::Text_Value, &TextField2::setTextByKey );
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::FieldSize_Value, &TextField2::setFieldSize  );
			BIN_CASE_ATTRIBUTE_METHODT( Protocol::VerticalAlign_Value, &TextField2::setVerticalAlign, EVerticalAlign );
			BIN_CASE_ATTRIBUTE_METHODT( Protocol::HorizontalAlign_Value, &TextField2::setHorizontalAlign, EHorizontalAlign );
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::CharOffset_Value, &TextField2::setCharOffset );
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::LineOffset_Value, &TextField2::setLineOffset );
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
			m_resourceFont->decrementReference();

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
