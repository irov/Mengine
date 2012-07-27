#	include "TextLine.h"

#	include "ResourceFont.h"
#	include "ResourceGlyph.h"

#	include "Glyph.h"

#	include "Interface/RenderSystemInterface.h"

#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TextLine::TextLine( float _height, float _charOffset )
		: m_charOffset(_charOffset)
		, m_height(_height)
		, m_length(0)
		, m_invalidateRenderLine(true)
		, m_offset(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void TextLine::initialize( const ResourceFont * _resource, const WString& _text )
	{
		WString::size_type text_size = _text.length();
		m_charsData.reserve( text_size );

		float totalKerning = 0.0f;
		
		for( WString::const_iterator
			it = _text.begin(), 
			it_end = _text.end();
		it != it_end; 
		++it )
		{	
			WString::value_type code = (*it);

			if ( _resource->hasGlyph( code ) == false )
			{
				MENGE_LOG_ERROR( "TextLine for resource %s invalid glyph %i"
					, _resource->getName().c_str()
					, code
					);

				continue;
			}

			const Glyph * glyph = _resource->getGlyph( code );

			const RenderTextureInterface * image = _resource->getTexture();
					
			CharData charData;
			charData.code = code;
			
			charData.uv = glyph->getUV();

			mt::vec2f imageInvSize;
			imageInvSize.x = 1.f / (float)image->getWidth();
			imageInvSize.y = 1.f / (float)image->getHeight();

			charData.uv.x *= imageInvSize.x;
			charData.uv.y *= imageInvSize.y;
			charData.uv.z *= imageInvSize.x;
			charData.uv.w *= imageInvSize.y;

			charData.ratio = glyph->getRatio();
			charData.offset = glyph->getOffset();
			
			charData.size = glyph->getSize();
			
			charData.size *= m_height;

			const ResourceGlyph * resourceGlyph = _resource->getResourceGlyph();

			float initSize = resourceGlyph->getHeight();
			charData.size /= initSize;
			
			if( m_charsData.empty() == false )
			{
				const CharData & prevChar = m_charsData.back();

				const Glyph * prevGlyph = _resource->getGlyph( prevChar.code );

				float kerning = prevGlyph->getKerning( charData.code );

				totalKerning += kerning;
			}

			charData.offset.x += totalKerning;
			m_charsData.push_back( charData );

			float height = m_height;
			float width = floorf( charData.ratio * height );
			m_length += width + m_charOffset;
		}

		m_length -= m_charOffset;
	}
	//////////////////////////////////////////////////////////////////////////
	int TextLine::getCharsDataSize() const
	{
		return m_charsData.size();
	}
	//////////////////////////////////////////////////////////////////////////
	float TextLine::getLength() const
	{
		return m_length;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextLine::invalidateRenderLine() const
	{
		m_invalidateRenderLine = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextLine::prepareRenderObject(	mt::vec2f & _offset
		, const mt::mat4f & _wm
		, const mt::vec4f & _uv
		, unsigned int _argb
		, bool _pixelsnap		
		, TVectorVertex2D& _renderObject ) const
	{
		if( m_invalidateRenderLine == true )
		{
			updateRenderLine_( _offset, _wm );
		}

		//_renderObject->vertices.clear();
		//_renderObject->passes[0].indicies.clear();
		//_renderObject->material.color = ;

		size_t renderObjectNum = _renderObject.size();

		_renderObject.resize( renderObjectNum + m_charsData.size() * 4 );

		for( TCharsData::const_iterator
			it_char = m_charsData.begin(), 
			it_char_end = m_charsData.end();
		it_char != it_char_end; 
		++it_char )
		{
			for( int i = 0; i != 4; ++i )
			{
				//_renderObject->vertices.push_back( TVertex() );
				Vertex2D & renderVertex = _renderObject[renderObjectNum + i];
				const mt::vec3f & charVertex = it_char->renderVertex[i];
				

				if( _pixelsnap )
				{
					renderVertex.pos[0] = floorf(charVertex.x + 0.5f);
					renderVertex.pos[1] = floorf(charVertex.y + 0.5f);
				}
				else
				{
					renderVertex.pos[0] = charVertex.x;
					renderVertex.pos[1] = charVertex.y;
				}

				renderVertex.pos[2] = charVertex.z;

				renderVertex.color = _argb;
			}

			const mt::vec4f & char_uv = it_char->uv;

			mt::vec4f total_uv;
			total_uv.x = _uv.x + (_uv.z - _uv.x) * char_uv.x;
			total_uv.y = _uv.y + (_uv.w - _uv.y) * char_uv.y;
			total_uv.z = _uv.x + (_uv.z - _uv.x) * char_uv.z;
			total_uv.w = _uv.y + (_uv.w - _uv.y) * char_uv.w;

			_renderObject[renderObjectNum + 0].uv[0] = total_uv.x;
			_renderObject[renderObjectNum + 0].uv[1] = total_uv.y;
			_renderObject[renderObjectNum + 1].uv[0] = total_uv.z;
			_renderObject[renderObjectNum + 1].uv[1] = total_uv.y;
			_renderObject[renderObjectNum + 2].uv[0] = total_uv.z;
			_renderObject[renderObjectNum + 2].uv[1] = total_uv.w;
			_renderObject[renderObjectNum + 3].uv[0] = total_uv.x;
			_renderObject[renderObjectNum + 3].uv[1] = total_uv.w;

			renderObjectNum += 4;
		}

		_offset.x += m_offset;

		return;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextLine::updateBoundingBox( mt::vec2f& _offset, const mt::mat4f & _wm, mt::box2f& _boundingBox )
	{
		if( m_charsData.empty() ) return;
		
		if( m_invalidateRenderLine == true )
		{
			updateRenderLine_( _offset, _wm );
		}

		mt::vec3f vb = m_charsData.front().renderVertex[0];
		mt::vec3f ve = m_charsData.back().renderVertex[2];

		mt::merge_box( _boundingBox, mt::box2f( vb.to_vec2f(), ve.to_vec2f() ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void TextLine::updateRenderLine_( mt::vec2f& _offset, const mt::mat4f & _wm ) const
	{
		for( TCharsData::iterator
			it_char = m_charsData.begin(), 
			it_char_end = m_charsData.end();
		it_char != it_char_end; 
		++it_char )
		{
			float width = floorf( it_char->ratio * m_height );

			mt::vec2f size = it_char->size;

			mt::vec2f offset = _offset + it_char->offset;
			mt::vec3f v3_offset(offset.x, offset.y, 0.f);
			mt::mul_v3_m4( it_char->renderVertex[0], v3_offset, _wm );
			mt::mul_v3_m4( it_char->renderVertex[1], v3_offset + mt::vec3f( size.x, 0.0f, 0.f ), _wm );
			mt::mul_v3_m4( it_char->renderVertex[2], v3_offset + mt::vec3f( size.x, size.y, 0.f ), _wm );
			mt::mul_v3_m4( it_char->renderVertex[3], v3_offset + mt::vec3f( 0.0f, size.y, 0.f ), _wm );

			// round coords
			//for( int i = 0; i != 4; ++i )
			//{
			//	it_char->renderVertex[i].x = ::floorf( it_char->renderVertex[i].x + 0.5f );
			//	it_char->renderVertex[i].y = ::floorf( it_char->renderVertex[i].y + 0.5f );
			//}

			_offset.x += width + m_charOffset;
		}

		m_offset = _offset.x;

		m_invalidateRenderLine = false;
	}
	//////////////////////////////////////////////////////////////////////////
}
