#	include "TTFText.h"

#	include "Interface/TextInterface.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TTFText::TTFText()
		: m_invalidateText( true )
		, m_invalidateFont( true )
		, m_invalidateVertices( true )
		, m_invalidateVerticesWM( true )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	TTFText::~TTFText()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void TTFText::setTextID( const ConstString & _key )
	{
		m_key = _key;

		this->invalidateText();
		this->invalidateVertices();
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & TTFText::getTextID() const
	{
		return m_key;
	}
	//////////////////////////////////////////////////////////////////////////
	void TTFText::updateFont_() const
	{
		m_invalidateFont = false;

		m_font = nullptr;

		if( m_fontName.empty() == true )
		{
			return;
		}

		m_font = TTFFONT_SERVICE( m_serviceProvider )
			->getFont( m_fontName );

		if( m_font == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("TTFText::updateFont_ '%s' can't found font '%s'"
				, this->getName().c_str()
				, m_fontName.c_str()
				);

			return;
		}

		this->invalidateVertices();
	}
	//////////////////////////////////////////////////////////////////////////
	void TTFText::updateText_()
	{
		m_invalidateText = false;

		const ConstString & textID = this->getTextID();

		const TextEntryInterface * textEntry = TEXT_SERVICE( m_serviceProvider )
			->getTextEntry( textID );

		const String & value = textEntry->getValue();

		const TTFFontInterfacePtr & font = this->getFont();

		m_text = font->prepareText( value );
	}
	//////////////////////////////////////////////////////////////////////////
	void TTFText::updateVertices_()
	{
		m_invalidateVertices = false;

		const TTFFontInterfacePtr & font = this->getFont();
		const U32String & text = this->getText();

		U32String::size_type text_size = text.size();

		m_vertexText.reserve( text_size * 4 );
		m_vertexText.clear();

		mt::vec2f offset(0.f, 0.f);

		for( U32String::const_iterator
			it = text.begin(),
			it_end = text.end();
		it != it_end;
		++it )
		{
			uint32_t ch = *it;

			const TTFGlyph * glyph = font->getGlyph( ch );

			RenderVertex2D v0;
			v0.position.x = offset.x + glyph->dx;
			v0.position.y = offset.y + glyph->dy;

			RenderVertex2D v1;
			v1.position.x = offset.x + glyph->ax;
			v1.position.y = offset.y + glyph->dy;

			RenderVertex2D v2;
			v2.position.x = offset.x + glyph->ax;
			v2.position.y = offset.y + glyph->ay;

			RenderVertex2D v3;
			v3.position.x = offset.x + glyph->dx;
			v3.position.y = offset.y + glyph->ay;

			offset.x += glyph->advance;
			
			m_vertexText.push_back( v0 );
			m_vertexText.push_back( v1 );
			m_vertexText.push_back( v2 );
			m_vertexText.push_back( v3 );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TTFText::updateVerticesWM_()
	{
		m_invalidateVerticesWM = false;

		const mt::mat4f & wm = this->getWorldMatrix();

		const TVectorRenderVertex2D & vertexText = this->getVertices();

		TVectorRenderVertex2D::const_iterator it = vertexText.begin();
		TVectorRenderVertex2D::const_iterator it_end = vertexText.end();

		TVectorRenderVertex2D::iterator it_w = m_vertexTextWM.begin();

		for( ; it != it_end; ++it, ++it_w )
		{
			const RenderVertex2D & vertex = *it;

			RenderVertex2D & vertex_w = *it_w;

			mt::mul_v3_v3_m4( vertex_w.position, vertex.position, wm );
		}
	}
}