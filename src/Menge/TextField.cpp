#     include "TextField.h" 

#     include "ObjectImplement.h"

#     include "XmlEngine.h"

#     include "RenderEngine.h"

#     include "ResourceManager.h"

#     include "ResourceFont.h"

#     include "ResourceImage.h"

#     include "LogEngine.h"

#     include "math/box2.h"

#     include "Utils.h"

#	  include <algorithm>

namespace     Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(TextField);
	//////////////////////////////////////////////////////////////////////////
	TextField::TextField()
		: m_resource( 0 )
		, m_length( 0.f, 0.f )
		, m_color( 1.f, 1.f, 1.f, 1.f )
		, m_outlineColor( 1.f, 1.f, 1.f, 1.f )
		, m_height( 0.f )
		, m_centerAlign( false )
		, m_rightAlign( false )
		, m_alignOffset( 0.f, 0.f )
		, m_maxWidth( 2048.f )
		, m_charOffset( 0.f )
		, m_lineOffset( 0.f )
		, m_outline( true )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	TextField::~TextField()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextField::_activate()
	{
		if( Node::_activate() == false )
		{
			return false;
		}

		setText( m_text );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_deactivate()
	{
		Node::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextField::_compile()
	{
		if( Node::_compile() == false )
		{
			return false;
		}

		m_resource = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceFont>( m_resourcename );

		if( m_resource == 0 )
		{
			MENGE_LOG( MENGE_TEXT("Warning: font '%s' can't find resource '%s'\n")
				, m_name.c_str()
				, m_resourcename.c_str() 
				);

			return false;
		}

		if( m_resource->isCompile() == false )
		{
			MENGE_LOG( MENGE_TEXT("Warning: font '%s' can't compile resource '%s'\n")
				, m_name.c_str()
				, m_resourcename.c_str() 
				);

			return false;
		}
	
		if( m_height == 0.0f )
		{
			m_height = m_resource->getInitSize();
		}

		if( m_lineOffset == 0.f )
		{
			m_lineOffset = m_height;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_release()
	{
		Node::_release();

		Holder<ResourceManager>::hostage()
			->releaseResource( m_resource );

		m_resource = 0;
	}
	///////////////////////////////////////////////////////////////////////////
	bool TextField::_checkVisibility( const Viewport & _viewPort )
	{
		const mt::mat3f & wm = getWorldMatrix();

		mt::box2f bbox;

		mt::set_box_from_oriented_extent( bbox, m_alignOffset, m_length, wm );

		bool result = _viewPort.testRectangle( bbox.min, bbox.max );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::loader( XmlElement * _xml )
	{
		Node::loader(_xml);

		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE( MENGE_TEXT("Font"), MENGE_TEXT("Name"), m_resourcename );
			XML_CASE_ATTRIBUTE_NODE( MENGE_TEXT("Text"), MENGE_TEXT("Value"), m_text );
			XML_CASE_ATTRIBUTE_NODE( MENGE_TEXT("Color"), MENGE_TEXT("Value"), m_color );
			XML_CASE_ATTRIBUTE_NODE( MENGE_TEXT("Height"), MENGE_TEXT("Value"), m_height );
			XML_CASE_ATTRIBUTE_NODE( MENGE_TEXT("CenterAlign"), MENGE_TEXT("Value"), m_centerAlign );
			XML_CASE_ATTRIBUTE_NODE( MENGE_TEXT("RightAlign"), MENGE_TEXT("Value"), m_rightAlign );
			XML_CASE_ATTRIBUTE_NODE( MENGE_TEXT("OutlineColor"), MENGE_TEXT("Value"), m_outlineColor );
			XML_CASE_ATTRIBUTE_NODE( MENGE_TEXT("Outline"), MENGE_TEXT("Value"), m_outline );
			XML_CASE_ATTRIBUTE_NODE( MENGE_TEXT("MaxWidth"), MENGE_TEXT("Value"), m_maxWidth );
			XML_CASE_ATTRIBUTE_NODE( MENGE_TEXT("CharOffset"), MENGE_TEXT("Value"), m_charOffset );
			XML_CASE_ATTRIBUTE_NODE( MENGE_TEXT("LineOffset"), MENGE_TEXT("Value"), m_lineOffset );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();
		
		for( std::list<TextLine>::iterator 
			it_line = m_lines.begin(),
			it_line_end = m_lines.end(); 
		it_line != it_line_end; 
		++it_line )
		{
			it_line->invalidateRenderLine();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_renderPass( ColourValue& _color, const RenderImageInterface * _renderImage )
	{
		mt::vec2f offset = mt::vec2f::zero_v2;

		const mt::mat3f & _wm = this->getWorldMatrix();

		for( std::list<TextLine>::iterator 
			it_line = m_lines.begin(),
			it_line_end = m_lines.end(); 
		it_line != it_line_end; 
		++it_line )
		{
			if( m_centerAlign )
			{
				m_alignOffset = mt::vec2f( -it_line->getLength() * 0.5f, 0 );
			}

			if( m_rightAlign )
			{
				m_alignOffset = mt::vec2f( -it_line->getLength(), 0 );
			}

			offset.x = m_alignOffset.x;

			it_line->renderLine(offset, _color, _renderImage);

			offset.y += m_lineOffset;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_render( unsigned int _debugMask )
	{
		const RenderImageInterface * outlineImage = m_resource->getOutlineImage();

		if( m_outline && outlineImage )
		{
			_renderPass( m_outlineColor, outlineImage );
		}

		const RenderImageInterface * renderImage = m_resource->getImage();

		_renderPass( m_color, renderImage );
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_update( float _timing )
	{
		Node::_update( _timing );

		if( m_colorTo.isStarted() )
		{
			m_outlineColorTo.update( _timing, &m_outlineColor );
			if( m_colorTo.update( _timing, &m_color ) )
			{
				callEvent( EVENT_COLOR_END, "(O)", this->getEmbedding() );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float TextField::getCharOffset() const
	{
		return m_charOffset;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setOutlineColor( const ColourValue& _color )
	{
		m_outlineColor = _color;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setMaxLen( float _len )
	{
		m_maxWidth = _len;
	}
	//////////////////////////////////////////////////////////////////////////
	const ColourValue& TextField::getOutlineColor() const
	{
		return m_outlineColor;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setText( const Text& _text )
	{
		if( m_resource == 0 )
		{
			MENGE_LOG( MENGE_TEXT("Warning: TextField without resource ('%s')")
				,m_resourcename.c_str() );
			return;
		}

		m_text = _text;

		createFormattedMessage_( m_text );
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::colorTo( const ColourValue& _color, float _time )
	{
		if( m_colorTo.isStarted() )
		{
			callEvent( EVENT_COLOR_STOP, "(O)", this->getEmbedding() );
		}

		if( m_colorTo.start( m_color, _color, _time, length_color ) == false )
		{
			m_color = _color;
			callEvent( EVENT_COLOR_END, "(O)", this->getEmbedding() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::alphaTo( float _alpha, float _time )
	{
		Node::alphaTo( _alpha, _time );
		
		ColourValue newColor = m_color;
		newColor.setA( _alpha );
		
		if( m_colorTo.isStarted() )
		{
			callEvent( EVENT_COLOR_STOP, "(O)", this->getEmbedding() );
			m_colorTo.stop();
		}

		if( m_colorTo.start( m_color, newColor, _time, length_color ) == false )
		{
			m_color = newColor;
			callEvent( EVENT_COLOR_END, "(O)", this->getEmbedding() );
		}

		newColor = m_outlineColor;
		newColor.setA( _alpha );

		if( m_outlineColorTo.start( m_outlineColor, newColor, _time, length_color ) == false )
		{
			m_outlineColor = newColor;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setColor( const ColourValue& _color )
	{
		m_color = _color;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setAlpha( float _alpha )
	{
		m_color.setA( _alpha );
		m_outlineColor.setA( _alpha );
	}
	//////////////////////////////////////////////////////////////////////////
	const ColourValue& TextField::getColor() const
	{
		return m_color;
	}
	//////////////////////////////////////////////////////////////////////////
	float TextField::getHeight() const
	{
		return m_height;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setHeight( float _height )
	{
		m_height = _height;
	}
	//////////////////////////////////////////////////////////////////////////
	const String& TextField::getText() const
	{
		return m_text;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f& TextField::getLength() const
	{
		return m_length;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::createFormattedMessage_( const Text& _text )
	{
		m_lines.clear();

		TStringVector lines;

		lines = Utils::split( _text, MENGE_TEXT("\n") );

		for(TStringVector::iterator line = lines.begin(); line != lines.end(); line++)
		{
			m_lines.push_back(TextLine(*this, m_resource,*line));
		}

		float maxlen = 0.0f;

		for(std::list<TextLine>::iterator line = m_lines.begin(); line != m_lines.end(); line++ )
		{
			maxlen = (std::max)( maxlen, line->getLength() );
		}

		m_length.x = maxlen;
		m_length.y = m_height * m_lines.size();
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_setListener()
	{
		Node::_setListener();

		registerEvent( EVENT_COLOR_END, ("onColorEnd"), m_listener );
		registerEvent( EVENT_COLOR_STOP, ("onColorStop"), m_listener );
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setLineOffset( float _offset )
	{
		m_lineOffset = _offset;
	}
	//////////////////////////////////////////////////////////////////////////
	float TextField::getLineOffset() const
	{
		return m_lineOffset;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setResource( const String& _resName )
	{
		if( m_resourcename != _resName )
		{
			m_resourcename = _resName;
			if( isCompile() )
			{
				recompile();
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setOutlineResource( const String& _outlineName )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextField::getCenterAlign()
	{
		return m_centerAlign;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::colorToStop()
	{
		m_colorTo.stop();
		m_outlineColorTo.stop();
		callEvent( EVENT_COLOR_STOP, "(O)", this->getEmbedding() );
	}
	//////////////////////////////////////////////////////////////////////////
}
