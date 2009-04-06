#     include "TextField.h" 

#     include "ObjectImplement.h"

#     include "XmlEngine.h"

#	include "RenderEngine.h"
#	include "RenderObject.h"

#     include "ResourceManager.h"

#     include "ResourceFont.h"

#     include "LogEngine.h"

#     include "TextManager.h"

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
		, m_outlineColor( 1.f, 1.f, 1.f, 1.f )
		, m_height( 0.f )
		, m_centerAlign( false )
		, m_rightAlign( false )
		, m_alignOffset( 0.f, 0.f )
		, m_maxWidth( 2048.f )
		, m_charOffset( 0.f )
		, m_lineOffset( 0.f )
		, m_outline( true )
		, m_renderObjectText( NULL )
		, m_renderObjectOutline( NULL )
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

		if( m_resourcename.empty() == true )
		{
			MENGE_LOG_ERROR( "Error: Font name is empty (TextField %s)"
				, m_name.c_str() );
			return false;
		}

		m_resource = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceFont>( m_resourcename );

		if( m_resource == 0 )
		{
			MENGE_LOG_ERROR( "Warning: font \"%s\" can't find resource "
			, m_name.c_str()
			, m_resourcename.c_str() );
			return false;
		}

		if( m_resource->isCompile() == false )
		{
			MENGE_LOG_ERROR( "Warning: font \"%s\" can't compile resource "
			, m_name.c_str()
			, m_resourcename.c_str() );
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

		m_renderObjectText = Holder<RenderEngine>::hostage()
								->createRenderObject();
		m_renderObjectOutline = Holder<RenderEngine>::hostage()
								->createRenderObject();

		m_renderObjectText->material.textureStages = 1;
		m_renderObjectText->material.primitiveType = PT_TRIANGLELIST;
		m_renderObjectText->material.blendSrc = BF_SOURCE_ALPHA;
		m_renderObjectText->material.blendDst = BF_ONE_MINUS_SOURCE_ALPHA;
		m_renderObjectText->material.textureStage[0].texture = m_resource->getImage();

		m_renderObjectOutline->material.textureStages = 1;
		m_renderObjectOutline->material.primitiveType = PT_TRIANGLELIST;
		m_renderObjectOutline->material.blendSrc = BF_SOURCE_ALPHA;
		m_renderObjectOutline->material.blendDst = BF_ONE_MINUS_SOURCE_ALPHA;
		if( m_resource->getOutlineImage() != NULL )
		{
			m_renderObjectOutline->material.textureStage[0].texture = m_resource->getOutlineImage();
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_release()
	{
		Holder<RenderEngine>::hostage()
			->releaseRenderObject( m_renderObjectText );
		Holder<RenderEngine>::hostage()
			->releaseRenderObject( m_renderObjectOutline );

		Node::_release();

		Holder<ResourceManager>::hostage()
			->releaseResource( m_resource );

		m_resource = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::loader( XmlElement * _xml )
	{
		Node::loader(_xml);

		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE( "Font", "Name", m_resourcename );
			XML_CASE_ATTRIBUTE_NODE( "Text", "Value", m_text );
			XML_CASE_ATTRIBUTE_NODE( "Height", "Value", m_height );
			XML_CASE_ATTRIBUTE_NODE( "CenterAlign", "Value", m_centerAlign );
			XML_CASE_ATTRIBUTE_NODE( "RightAlign", "Value", m_rightAlign );
			XML_CASE_ATTRIBUTE_NODE( "OutlineColor", "Value", m_outlineColor );
			XML_CASE_ATTRIBUTE_NODE( "Outline", "Value", m_outline );
			XML_CASE_ATTRIBUTE_NODE( "MaxWidth", "Value", m_maxWidth );
			XML_CASE_ATTRIBUTE_NODE( "CharOffset", "Value", m_charOffset );
			XML_CASE_ATTRIBUTE_NODE( "LineOffset", "Value", m_lineOffset );
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
	void TextField::_renderPass( ColourValue& _color, RenderObject* _renderObject )
	{
		_renderObject->vertices.clear();
		_renderObject->material.indicies.clear();
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
			else if( m_rightAlign )
			{
				m_alignOffset = mt::vec2f( -it_line->getLength(), 0 );
			}
			else
			{
				m_alignOffset = mt::vec2f( 0.0f, 0.0f );
			}

			offset.x = m_alignOffset.x;

			it_line->prepareRenderObject( offset, _color.getAsARGB(), _renderObject );

			offset.y += m_lineOffset;
		}
		Holder<RenderEngine>::hostage()
			->renderObject( _renderObject );
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_render( unsigned int _debugMask )
	{
		Node::_render( _debugMask );

		ColourValue& wColor = getWorldColor();
		m_outlineColor.setA( wColor.getA() );

		if( m_outline && m_renderObjectOutline->material.textureStage[0].texture != NULL )
		{
			_renderPass( m_outlineColor, m_renderObjectOutline );
		}

		_renderPass( wColor, m_renderObjectText );
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
	void TextField::setText( const String& _text )
	{
		if( m_resource == 0 )
		{
			MENGE_LOG_ERROR( "Warning: TextField without resource \"%s\""
				, m_resourcename.c_str() );
			return;
		}

		m_text = _text;

		createFormattedMessage_( m_text );
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
	const mt::vec2f& TextField::getLength()
	{
		const mt::box2f& bb = getBoundingBox();
		static mt::vec2f len;
		len = bb.maximum - bb.minimum;
		return len;
		//return m_length;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::createFormattedMessage_( const String& _text )
	{
		m_lines.clear();

		TStringVector lines;

		//lines = Utils::split( _text, "\n\\n" );
		lines = Utils::split( _text, "\n" );

		for(TStringVector::iterator line = lines.begin(); line != lines.end(); line++)
		{
			TextLine textLine( *this, m_resource, *line );
			if( textLine.getLength() > m_maxWidth )
			{
				TStringVector words = Utils::split( *line, " " );
			
				String newLine = words.front();
				words.erase( words.begin() );
				while( words.empty() == false )
				{
					TextLine tl( *this, m_resource, String( newLine + String( " " ) + ( *words.begin() ) ) );
					if( tl.getLength() > m_maxWidth )
					{
						m_lines.push_back( TextLine( *this, m_resource, newLine ) );
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
				m_lines.push_back( TextLine( *this, m_resource, newLine ) );
			}
			else
			{
				m_lines.push_back( textLine );
			}
		}

		float maxlen = 0.0f;

		for(std::list<TextLine>::iterator line = m_lines.begin(); line != m_lines.end(); line++ )
		{
			maxlen = (std::max)( maxlen, line->getLength() );
		}

		m_length.x = maxlen;
		m_length.y = m_height * m_lines.size();

		invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_setListener()
	{
		Node::_setListener();

		//registerEvent( EVENT_COLOR_END, ("onColorEnd"), m_listener );
		//registerEvent( EVENT_COLOR_STOP, ("onColorStop"), m_listener );
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
	void TextField::_updateBoundingBox( mt::box2f & _boundingBox )
	{
		Node::_updateBoundingBox( _boundingBox );

		mt::vec2f offset = mt::vec2f::zero_v2;
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
			else if( m_rightAlign )
			{
				m_alignOffset = mt::vec2f( -it_line->getLength(), 0 );
			}
			else
			{
				m_alignOffset = mt::vec2f( 0.0f, 0.0f );
			}

			offset.x = m_alignOffset.x;

			it_line->updateBoundingBox( _boundingBox, offset );

			offset.y += m_lineOffset;
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setCenterAlign( bool _centerAlign )
	{
		m_centerAlign = _centerAlign;
		if( m_centerAlign == true )
		{
			m_rightAlign = false;
		}
		setText( m_text );
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setCharOffset( float _offset )
	{
		m_charOffset = _offset;
		setText( m_text );
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setTextByKey( const String& _key )
	{
		TextManager::TextEntry textEntry = Holder<TextManager>::hostage()
			->getTextEntry( _key );
		if( textEntry.font.empty() == false && textEntry.font != m_resourcename )
		{
			setResource( textEntry.font );
		}
		if( textEntry.charOffset != 0.0f && textEntry.charOffset != m_charOffset )
		{
			setCharOffset( textEntry.charOffset );
		}
		if( textEntry.lineOffset == 0.0f )
		{
			textEntry.lineOffset = m_height;
		}
		if( textEntry.lineOffset != m_lineOffset )
		{
			setLineOffset( textEntry.lineOffset );
		}
		if( isCompile() == false )
		{
			m_text = textEntry.text;
		}
		else
		{
			setText( textEntry.text );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextField::getRightAlign()
	{
		return m_rightAlign;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setRightAlign( bool _rightAlign )
	{
		m_rightAlign = _rightAlign;
		if( m_rightAlign == true )
		{
			m_centerAlign = false;
		}
		setText( m_text );
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextField::getLeftAlign()
	{
		return ( (m_centerAlign || m_rightAlign) == false );
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setLeftAlign( bool _leftAlign )
	{
		if( _leftAlign == true )
		{
			m_centerAlign = false;
			m_rightAlign = false;
		}
		setText( m_text );
	}
	//////////////////////////////////////////////////////////////////////////
}
