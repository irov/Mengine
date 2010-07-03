#     include "TextField.h" 

#     include "XmlEngine.h"

#	include "RenderEngine.h"
#	include "Material.h"

#     include "ResourceManager.h"

#     include "ResourceFont.h"

#	include "Logger/Logger.h"

#     include "TextManager.h"

#     include "math/box2.h"

#	include "Utils/Core/String.h"

#	  include <algorithm>

namespace Menge
{
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
		, m_materialText( NULL )
		, m_materialOutline( NULL )
		, m_invalidateVertices( true )
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
				, getName().c_str() 
				);

			return false;
		}

		m_resource = ResourceManager::get()
			->getResourceT<ResourceFont>( m_resourcename );

		if( m_resource == 0 )
		{
			MENGE_LOG_ERROR( "Warning: font '%s' can't find resource '%s'"
				, getName().c_str()
				, m_resourcename.c_str() 
				);

			return false;
		}

		if( m_resource->isCompile() == false )
		{
			MENGE_LOG_ERROR( "Warning: font '%s' can't compile resource '%s'"
				, getName().c_str()
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

		m_materialText = RenderEngine::get()
								->createMaterial();

		m_materialOutline = RenderEngine::get()
								->createMaterial();

		//m_materialText->textureStages = 1;
		m_materialText->blendSrc = BF_SOURCE_ALPHA;
		m_materialText->blendDst = BF_ONE_MINUS_SOURCE_ALPHA;
		//m_materialText->textureStage[0].texture = m_resource->getImage();
		m_materialText->textureStage[0].colorOp = TOP_MODULATE;
		m_materialText->textureStage[0].alphaOp = TOP_MODULATE;

		//m_materialOutline->textureStages = 1;
		m_materialOutline->blendSrc = BF_SOURCE_ALPHA;
		m_materialOutline->blendDst = BF_ONE_MINUS_SOURCE_ALPHA;
		m_materialOutline->textureStage[0].colorOp = TOP_MODULATE;
		m_materialOutline->textureStage[0].alphaOp = TOP_MODULATE;
		/*if( m_resource->getOutlineImage() != NULL )
		{
			m_materialOutline->textureStage[0].texture = m_resource->getOutlineImage();
		}*/

		if( m_text.empty() == false )
		{
			createFormattedMessage_( m_text );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_release()
	{
		RenderEngine::get()
			->releaseMaterial( m_materialText );

		RenderEngine::get()
			->releaseMaterial( m_materialOutline );

		Node::_release();

		ResourceManager::get()
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
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Text", "Value", &TextField::setTextByKey );
			//XML_CASE_ATTRIBUTE_NODE( "Text", "Value", m_text );
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
		
		for( TListTextLine::iterator 
			it_line = m_lines.begin(),
			it_line_end = m_lines.end(); 
		it_line != it_line_end; 
		++it_line )
		{
			it_line->invalidateRenderLine();
		}

		m_invalidateVertices = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::updateVertexData_( const ColourValue & _color, TVertex2DVector& _vertexData )
	{
		_vertexData.clear();

		mt::vec2f offset = mt::vec2f::zero_v2;

		const mt::mat3f & _wm = this->getWorldMatrix();

		for( TListTextLine::iterator 
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

			it_line->prepareRenderObject( offset, _color.getAsARGB(), _vertexData );

			offset.y += m_lineOffset;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_render( Camera2D * _camera )
	{
		Node::_render( _camera );

		if( m_outline && m_resource->getOutlineImage() != NULL )
		{
			TVertex2DVector & outlineVertices = getOutlineVertices();

			Texture* outlineTexture = m_resource->getOutlineImage();
			RenderEngine::get()
				->renderObject2D( m_materialOutline, &outlineTexture, 1, &(outlineVertices[0]), outlineVertices.size(), LPT_QUAD );
		}

		TVertex2DVector & textVertices = getTextVertices();

		Texture* fontTexture = m_resource->getImage();
		RenderEngine::get()
			->renderObject2D( m_materialText, &fontTexture, 1, &(textVertices[0]), textVertices.size(), LPT_QUAD );
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
		m_text = _text;
		
		m_key.clear();

		if( m_resource )
		{
			createFormattedMessage_( m_text );
		}
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
		//const mt::box2f& bb = getBoundingBox();
		//static mt::vec2f len;
		//len = bb.maximum - bb.minimum;
		//return len;
		return m_length;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::createFormattedMessage_( const String& _text )
	{
		m_lines.clear();

		TVectorString lines;

		//lines = Utils::split( _text, "\n\\n" );
		Utils::split( lines, _text, false, "\n" );

		for(TVectorString::iterator line = lines.begin(); line != lines.end(); line++)
		{
			TextLine textLine( *this, m_resource, *line );
			if( textLine.getLength() > m_maxWidth )
			{
				TVectorString words;
				Utils::split( words, *line, false, " " );
			
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

		for(TListTextLine::iterator 
			it = m_lines.begin(),
			it_end = m_lines.end(); 
		it != it_end;
		++it )
		{
			maxlen = (std::max)( maxlen, it->getLength() );
		}

		m_length.x = maxlen;
		m_length.y = m_height * m_lines.size();

		m_invalidateVertices = true;
		invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_setListener( PyObject * _listener )
	{
		Node::_setListener( _listener );

		//registerEvent( EVENT_COLOR_END, ("onColorEnd"), _listener );
		//registerEvent( EVENT_COLOR_STOP, ("onColorStop"), _listener );
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
	void TextField::setResource( const ConstString& _resName )
	{
		if( m_resourcename == _resName )
		{
			return;

		}
		
		m_resourcename = _resName;
		
		m_height = 0.0f;	// reset height

		recompile();
		//	if( isCompile() == true )
		//	{
		//		//recompile();
		//		_release();
		//		if( _compile() == false )
		//		{
		//			release();
		//		}
		//	}
		//}
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & TextField::getResource() const
	{
		return m_resourcename;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextField::getCenterAlign() const
	{
		return m_centerAlign;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_updateBoundingBox( mt::box2f & _boundingBox )
	{
		Node::_updateBoundingBox( _boundingBox );

		mt::vec2f offset = mt::vec2f::zero_v2;
		for( TListTextLine::iterator 
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
	void TextField::_invalidateColor()
	{
		m_invalidateVertices = true;
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
	void TextField::setTextByKey( const ConstString& _key )
	{
		if( _key.empty() == true )
		{
			return;
		}

		TextManager::TextEntry textEntry = 
			TextManager::get()->getTextEntry( _key );

		if( ( textEntry.font.empty() == false ) && ( textEntry.font != m_resourcename ) )
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

		m_key = _key;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & TextField::getTextKey() const
	{
		return m_key;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextField::getRightAlign() const
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
	bool TextField::getLeftAlign() const
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
	void TextField::invalidateVertices()
	{
		m_invalidateVertices = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::updateVertices()
	{
		m_invalidateVertices = false;

		const ColourValue & color = getWorldColor();

		m_outlineColor.setA( color.getA() );

		if( m_outline && m_resource->getOutlineImage() != NULL )
		{
			updateVertexData_( m_outlineColor, m_vertexDataOutline );
		}

		updateVertexData_( color, m_vertexDataText );
	}
}
