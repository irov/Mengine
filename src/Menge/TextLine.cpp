#	include "TextLine.h"
#	include "ResourceFont.h"
#   include "RenderObject.h"
#	include "TextField.h"
#	include "LogEngine.h"
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TextLine::TextLine( TextField & _textField, const ResourceFont * _resource, const String& _text )
	: m_length(0)
	, m_invalidateRenderLine(true)
	, m_offset(0)
	, m_textField(_textField)
	{
		for( String::const_iterator
			it = _text.begin(), 
			it_end = _text.end();
		it != it_end; 
		/*++it*/ )
		{
			CharData charData;

			unsigned char byte = (*it);
			unsigned char mask = 0x80;
			int i = 0;
			charData.code = 0;
			if( ( byte & mask ) == 0 )
			{
				charData.code = byte;
				it++;
			}
			else
			{
				while( ( byte & (mask >> i) ) != 0 )
				{
					if( it == it_end )
					{
						MENGE_LOG_ERROR( "TextField::setText: Invalid Text Encoding (not utf-8)" );
						break;
					}
					unsigned char glyphPart = (*it);
					charData.code |= ( glyphPart << (i*8) );
					it++;
					i++;
				}
			}

			//charData.code = *it;
			/*if( charData.code == 32 )
			{
				charData.uv = mt::vec4f( 0.0f, 0.0f, 0.0f, 0.0f );
				charData.ratio = 0.5f;
			}
			else*/
			{
				charData.uv = _resource->getUV( charData.code );
				charData.ratio = _resource->getCharRatio( charData.code );
			}

			charsData.push_back( charData );

			m_length += static_cast<int>( charData.ratio * m_textField.getHeight() ) + m_textField.getCharOffset();
		}
		m_length -= m_textField.getCharOffset();
	}	
	//////////////////////////////////////////////////////////////////////////
	float TextLine::getLength() const
	{
		return m_length;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextLine::invalidateRenderLine()
	{
		m_invalidateRenderLine = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextLine::prepareRenderObject(	mt::vec2f & offset,
								uint32 _argb, 
								RenderObject* _renderObject
								)
	{
		if( m_invalidateRenderLine == true )
		{
			updateRenderLine_( offset );
		}

		//_renderObject->vertices.clear();
		//_renderObject->passes[0].indicies.clear();
		//_renderObject->material.color = _color;

		std::size_t verticesNum = _renderObject->vertices.size();

		for( TCharsData::const_iterator
			it_char = charsData.begin(), 
			it_char_end = charsData.end();
		it_char != it_char_end; 
		++it_char )
		{
			_renderObject->material.indicies.push_back( 0 + verticesNum );
			_renderObject->material.indicies.push_back( 3 + verticesNum );
			_renderObject->material.indicies.push_back( 1 + verticesNum );
			_renderObject->material.indicies.push_back( 1 + verticesNum );
			_renderObject->material.indicies.push_back( 3 + verticesNum );
			_renderObject->material.indicies.push_back( 2 + verticesNum );
			for( int i = 0; i < 4; ++i )
			{
				_renderObject->vertices.push_back( TVertex() );
				_renderObject->vertices[verticesNum].pos[0] = it_char->renderVertex[i].x;
				_renderObject->vertices[verticesNum].pos[1] = it_char->renderVertex[i].y;
				_renderObject->vertices[verticesNum].color = _argb;
				++verticesNum;
			}
			_renderObject->vertices[verticesNum-4].uv[0] = it_char->uv.x;
			_renderObject->vertices[verticesNum-4].uv[1] = it_char->uv.y;
			_renderObject->vertices[verticesNum-3].uv[0] = it_char->uv.z;
			_renderObject->vertices[verticesNum-3].uv[1] = it_char->uv.y;
			_renderObject->vertices[verticesNum-2].uv[0] = it_char->uv.z;
			_renderObject->vertices[verticesNum-2].uv[1] = it_char->uv.w;
			_renderObject->vertices[verticesNum-1].uv[0] = it_char->uv.x;
			_renderObject->vertices[verticesNum-1].uv[1] = it_char->uv.w;

		}

		offset.x += m_offset;

		return;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextLine::updateBoundingBox( mt::box2f& _boundingBox, mt::vec2f& _offset )
	{
		if( charsData.empty() ) return;
		if( m_invalidateRenderLine == true )
		{
			updateRenderLine_( _offset );
		}
		mt::vec2f vb = charsData.front().renderVertex[0];
		mt::vec2f ve = charsData.back().renderVertex[2];
		mt::merge_box( _boundingBox, mt::box2f( vb, ve ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void TextLine::updateRenderLine_( mt::vec2f& _offset )
	{
		const mt::mat3f & _wm = m_textField.getWorldMatrix();

		for( TCharsData::iterator
			it_char = charsData.begin(), 
			it_char_end = charsData.end();
		it_char != it_char_end; 
		++it_char )
		{
			float width = floorf( it_char->ratio * m_textField.getHeight() );

			mt::vec2f size( width, m_textField.getHeight() );

			mt::mul_v2_m3( it_char->renderVertex[0], _offset, _wm );
			mt::mul_v2_m3( it_char->renderVertex[1], _offset + mt::vec2f( size.x, 0.0f ), _wm );
			mt::mul_v2_m3( it_char->renderVertex[2], _offset + size, _wm );
			mt::mul_v2_m3( it_char->renderVertex[3], _offset + mt::vec2f( 0.0f, size.y ), _wm );

			// round coords
			for( int i = 0; i < 4; i++ )
			{
				it_char->renderVertex[i].x = ::floorf( it_char->renderVertex[i].x + 0.5f );
				it_char->renderVertex[i].y = ::floorf( it_char->renderVertex[i].y + 0.5f );
			}

			_offset.x += width + m_textField.getCharOffset();
		}

		m_offset = _offset.x;

		m_invalidateRenderLine = false;
	}
	//////////////////////////////////////////////////////////////////////////
}
