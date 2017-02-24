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
    void TTFText::setFontName( const ConstString & _fontName )
    {
        m_fontName = _fontName;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & TTFText::getFontName() const
    {
        return m_fontName;
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

        m_vertexText.clear();
        m_chunks.clear();

		const U32String & text = this->getText();

        if( text.empty() == true )
        {
            return;
        }

		U32String::size_type text_size = text.size();

		m_vertexText.reserve( text_size * 4 );

        const TTFFontInterfacePtr & font = this->getFont();

		mt::vec2f offset(0.f, 0.f);

        Chunk chunk;
        chunk.begin = 0;
        chunk.count = 0;
        chunk.material = nullptr;

        const TTFGlyph * glyph0 = font->getGlyph( text[0] );

        ConstString materialName;

        switch( m_blendMode )
        {
        case EMB_NORMAL:
            {                
                materialName = RENDERMATERIAL_SERVICE( m_serviceProvider )
                    ->getMaterialName( EM_TEXTURE_BLEND );
            }break;
        };

        RenderMaterialInterfacePtr material0 = RENDERMATERIAL_SERVICE( m_serviceProvider )
            ->getMaterial( materialName, PT_TRIANGLELIST, 1, &glyph0->texture );

        chunk.material = material0;

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
            v0.position.z = 0.f;
            v0.color = 0xFFFFFFFF;
            v0.uv[0] = glyph->uv.p0;

			RenderVertex2D v1;
			v1.position.x = offset.x + glyph->ax;
			v1.position.y = offset.y + glyph->dy;
            v1.position.z = 0.f;
            v1.color = 0xFFFFFFFF;
            v1.uv[0] = glyph->uv.p1;

			RenderVertex2D v2;
			v2.position.x = offset.x + glyph->ax;
			v2.position.y = offset.y + glyph->ay;
            v2.position.z = 0.f;
            v2.color = 0xFFFFFFFF;
            v2.uv[0] = glyph->uv.p2;

			RenderVertex2D v3;
			v3.position.x = offset.x + glyph->dx;
			v3.position.y = offset.y + glyph->ay;
            v3.position.z = 0.f;
            v3.color = 0xFFFFFFFF;
            v3.uv[0] = glyph->uv.p3;

			offset.x += glyph->advance;
			
			m_vertexText.push_back( v0 );
			m_vertexText.push_back( v1 );
			m_vertexText.push_back( v2 );
			m_vertexText.push_back( v3 );

            RenderMaterialInterfacePtr material = RENDERMATERIAL_SERVICE( m_serviceProvider )
                ->getMaterial( materialName, PT_TRIANGLELIST, 1, &glyph->texture );

            if( chunk.material == material )
            {
                chunk.count += 4;
            }
            else
            {
                m_chunks.push_back( chunk );

                chunk.begin = chunk.begin + chunk.count;
                chunk.count = 4;
                chunk.material = material;
            }
		}

        if( chunk.count != 0 )
        {
            m_chunks.push_back( chunk );
        }
	}
	//////////////////////////////////////////////////////////////////////////
	void TTFText::updateVerticesWM_()
	{
		m_invalidateVerticesWM = false;

		const mt::mat4f & wm = this->getWorldMatrix();

		const TVectorRenderVertex2D & vertexText = this->getVertices();

        TVectorRenderVertex2D::size_type vertex_size = vertexText.size();

        m_vertexTextWM.resize( vertex_size );

		TVectorRenderVertex2D::const_iterator it = vertexText.begin();
		TVectorRenderVertex2D::const_iterator it_end = vertexText.end();

		TVectorRenderVertex2D::iterator it_w = m_vertexTextWM.begin();

		for( ; it != it_end; ++it, ++it_w )
		{
			const RenderVertex2D & vertex = *it;

			RenderVertex2D & vertex_w = *it_w;

			mt::mul_v3_v3_m4( vertex_w.position, vertex.position, wm );
            vertex_w.color = vertex.color;
            vertex_w.uv[0] = vertex.uv[0];
		}
	}
    //////////////////////////////////////////////////////////////////////////
    void TTFText::_render( Menge::RenderServiceInterface * _renderService, const RenderObjectState * _state )
    {
        const TVectorRenderVertex2D & textVertices = this->getVerticesWM();

        const TVectorRenderVertex2D::value_type * vertices = &textVertices.front();

        const mt::box2f & bb = this->getBoundingBox();

        for( TVectorChunks::const_iterator
            it = m_chunks.begin(),
            it_end = m_chunks.end();
            it != it_end;
            ++it )
        {
            const Chunk & chunk = *it;

            const TVectorRenderVertex2D::value_type * chunk_vertices = vertices + chunk.begin;            

            _renderService
                ->addRenderQuad( _state, chunk.material, chunk_vertices, chunk.count, &bb, false );
        }                
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFText::_invalidateWorldMatrix()
    {
        Node::_invalidateWorldMatrix();

        this->invalidateVerticesWM();
    }
    //////////////////////////////////////////////////////////////////////////
    RenderMaterialInterfacePtr TTFText::_updateMaterial() const
    {
        return nullptr;
    }
}