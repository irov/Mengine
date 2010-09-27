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
#include "XmlEngine.h"
#include "RenderEngine.h"
#include "TextManager.h"
#include "Logger/Logger.h"
#include "Utils/Core/String.h"

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
		m_invalidateLines = true;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f& TextField2::getFieldSize() const
	{
		return m_fieldSize;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField2::setVerticalAlign( VerticalAlign _align )
	{
		if( m_verticalAlign != _align )
		{
			m_verticalAlign = _align;
			m_invalidateVertices = true;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	TextField2::VerticalAlign TextField2::getVerticalAlign() const
	{
		return m_verticalAlign;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField2::setHorizontalAlign( HorizontalAlign _align )
	{
		if( m_horizontalAlign != _align )
		{
			m_horizontalAlign = _align;
			m_invalidateVertices = true;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	TextField2::HorizontalAlign TextField2::getHorizontalAlign() const
	{
		return m_horizontalAlign;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField2::setText( const String& _text )
	{
		if( m_text != _text )
		{
			m_text = _text;
			m_invalidateLines = true;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const String& TextField2::getText() const
	{
		return m_text;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField2::setTextByKey( const String& _key )
	{
		const TextManager::TextEntry& textEntry = TextManager::hostage()->getTextEntry( _key );
		setFont( textEntry.font );
		setCharOffset( textEntry.charOffset );
		setLineOffset( textEntry.lineOffset );
		setText( textEntry.text );
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField2::setFont( const String& _font )
	{
		if( m_font != _font )
		{
			m_font = _font;
			if( isCompile() == true )
			{
				ResourceManager::hostage()
					->releaseResource( m_resourceFont );
				m_resourceFont = ResourceManager::hostage()->getResourceT<ResourceFont>( m_font );
			}
		
			m_invalidateLines = true;
		}
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
		m_invalidateLines = true;
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
		Node::_render( _camera );

		if( m_invalidateLines == true )
		{
			rebuildLines_();
		}
		if( m_invalidateVertices == true )
		{
			updateVertices_();
		}

		Texture* fontTexture = m_resourceFont->getImage();
		Holder<RenderEngine>::hostage()
			->renderObject2D( m_materialText, &fontTexture, 1, &(m_vertices[0]), m_vertices.size(), LPT_QUAD );
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextField2::_compile()
	{
		m_materialText = Holder<RenderEngine>::hostage()
			->createMaterial();

		//m_materialText->textureStages = 1;
		m_materialText->blendSrc = BF_SOURCE_ALPHA;
		m_materialText->blendDst = BF_ONE_MINUS_SOURCE_ALPHA;
		//m_materialText->textureStage[0].texture = m_resource->getImage();
		m_materialText->textureStage[0].colorOp = TOP_MODULATE;
		m_materialText->textureStage[0].alphaOp = TOP_MODULATE;

		if( m_font.empty() == false )
		{
			m_resourceFont = ResourceManager::hostage()->getResourceT<ResourceFont>( m_font );
		}
		m_invalidateLines = true;

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
		RenderEngine::hostage()->releaseMaterial( m_materialText );
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField2::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();
		m_invalidateVertices = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField2::_updateBoundingBox( mt::box2f & _boundingBox )
	{
		_boundingBox.minimum = getWorldPosition();
		mt::mul_v2_m3( _boundingBox.maximum, m_fieldSize, getWorldMatrix() );
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField2::_invalidateColor()
	{
		Node::_invalidateColor();
		m_invalidateVertices = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField2::updateVertices_()
	{
		m_invalidateVertices = false;
		m_vertices.resize( m_text.length() * 4 );
		if( m_lines.empty() == true )
		{
			return;
		}
		float textHeight = m_lines.size() * (m_resourceFont->getInitSize() + m_lineOffset) - m_lineOffset;
		mt::vec2f textStart( 0.0f, 0.0f );
		textHeight = std::min( textHeight, m_fieldSize.y );
		if( m_verticalAlign == VA_CENTER )
		{
			textStart.y = (m_fieldSize.y - textHeight) * 0.5f;
		}
		else if( m_verticalAlign == VA_BOTTOM )
		{
			textStart.y = m_fieldSize.y - textHeight;
		}
		uint32 color = getWorldColor().getAsARGB();
		Vertex2D* vertices = &(m_vertices[0]);
		const mt::mat3f& wm = getWorldMatrix();
		for( TVectorString::iterator it_line = m_lines.begin(), it_line_end = m_lines.end();
			it_line != it_line_end;
			++it_line )
		{
			String& line = (*it_line);
			float lineLength = s_calcLength( line, m_charOffset, m_resourceFont );
			if( m_horizontalAlign == HA_LEFT )
			{
				textStart.x = 0.0f;
			}
			else if( m_horizontalAlign == HA_CENTER )
			{
				textStart.x = (m_fieldSize.x - lineLength) * 0.5f;
			}
			else if( m_horizontalAlign == HA_RIGHT )
			{
				textStart.x = m_fieldSize.x - lineLength;
			}
			for( String::iterator it = line.begin(), it_end = line.end();
				it != it_end;
				/*++it*/ )
			{
				unsigned char byte = (*it);
				unsigned char mask = 0x80;
				int i = 0;
				unsigned int code = 0;
				if( ( byte & mask ) == 0 )
				{
					code = byte;
					it++;
				}
				else
				{
					while( ( byte & (mask >> i) ) != 0 )
					{
						if( it == it_end )
						{
							MENGE_LOG_ERROR( "TextField2::setText: Invalid Text Encoding (not utf-8)" );
							break;
						}
						unsigned char glyphPart = (*it);
						code |= ( glyphPart << (i*8) );
						it++;
						i++;
					}
				}
				const mt::vec4f& uv = m_resourceFont->getUV( code );
				const mt::vec2f& size = m_resourceFont->getSize( code );
				const mt::vec2f& glyph_offset = m_resourceFont->getOffset( code );
				float char_width = m_resourceFont->getCharRatio( code ) * m_resourceFont->getInitSize();

				mt::vec2f renderVertex[4];
				mt::vec2f offset = textStart + glyph_offset;
				mt::mul_v2_m3( renderVertex[0], offset, wm );
				mt::mul_v2_m3( renderVertex[1], offset + mt::vec2f( size.x, 0.0f ), wm );
				mt::mul_v2_m3( renderVertex[2], offset + size, wm );
				mt::mul_v2_m3( renderVertex[3], offset + mt::vec2f( 0.0f, size.y ), wm );

				vertices->color = color;
				vertices->uv[0] = uv.x;
				vertices->uv[1] = uv.y;
				vertices->pos[0] = renderVertex[0].x;
				vertices->pos[1] = renderVertex[0].y;
				++vertices;
				vertices->color = color;
				vertices->uv[0] = uv.z;
				vertices->uv[1] = uv.y;
				vertices->pos[0] = renderVertex[1].x;
				vertices->pos[1] = renderVertex[1].y;
				++vertices;
				vertices->color = color;
				vertices->uv[0] = uv.z;
				vertices->uv[1] = uv.w;
				vertices->pos[0] = renderVertex[2].x;
				vertices->pos[1] = renderVertex[2].y;
				++vertices;
				vertices->color = color;
				vertices->uv[0] = uv.x;
				vertices->uv[1] = uv.w;
				vertices->pos[0] = renderVertex[3].x;
				vertices->pos[1] = renderVertex[3].y;
				++vertices;

				textStart.x += char_width + m_charOffset;
			}
			textStart.y += m_resourceFont->getInitSize() + m_lineOffset;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField2::rebuildLines_()
	{
		m_invalidateLines = false;
		m_lines.clear();

		TVectorString lines;
		Utils::split( lines, m_text, false, "\n" );

		for(TVectorString::iterator it_line = lines.begin(), it_line_end = lines.end();
			it_line != it_line_end; ++it_line)
		{
			String& line = (*it_line);
			float lineLength = s_calcLength( line, m_charOffset, m_resourceFont );
			if( lineLength > m_fieldSize.x )
			{
				TVectorString words;
				Utils::split( words, line, false, " " );

				String newLine = words.front();
				words.erase( words.begin() );
				while( words.empty() == false )
				{
					float testLineLength = s_calcLength( String( newLine + String( " " ) + ( *words.begin() ) ), m_charOffset, m_resourceFont );
					if( testLineLength > m_fieldSize.x )
					{
						m_lines.push_back( newLine );
						newLine.clear();
						newLine = words.front();
						words.erase( words.begin() );
					}
					else
					{
						newLine += String( " " ) + ( *words.begin() );
						words.erase( words.begin() );
					}
				}
				m_lines.push_back( newLine );
			}
			else
			{
				m_lines.push_back( line );
			}
		}


		m_invalidateVertices = true;
		invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	float TextField2::s_calcLength( const String& _text, float _charOffset, ResourceFont* _font )
	{
		float length = 0.0f;
		for( String::const_iterator it = _text.begin(), it_end = _text.end();
				it != it_end;
				/*++it*/ )
		{
			unsigned char byte = (*it);
			unsigned char mask = 0x80;
			int i = 0;
			unsigned int code = 0;
			if( ( byte & mask ) == 0 )
			{
				code = byte;
				it++;
			}
			else
			{
				while( ( byte & (mask >> i) ) != 0 )
				{
					if( it == it_end )
					{
						MENGE_LOG_ERROR( "TextField2::setText: Invalid Text Encoding (not utf-8)" );
						break;
					}
					unsigned char glyphPart = (*it);
					code |= ( glyphPart << (i*8) );
					it++;
					i++;
				}
			}
			length += _font->getSize( code ).x + _charOffset;
		}
		if( _text.empty() == false )
		{
			length -= _charOffset;
		}
		return length;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
