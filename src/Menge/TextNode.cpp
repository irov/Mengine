#	include "TextNode.h" 

#	include "ObjectImplement.h"

#	include "RenderEngine.h"

#	include "Viewport.h"

#	include "XmlParser/XmlParser.h"

#	include "Interface/RenderSystemInterface.h"

#	include "ResourceManager.h"

#	include "math/bv.h"

#	include "ResourceFont.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(TextNode);
	//////////////////////////////////////////////////////////////////////////
	TextNode::TextNode()
	{}
	//////////////////////////////////////////////////////////////////////////
	TextNode::~TextNode()
	{}
	///////////////////////////////////////////////////////////////////////////
	bool TextNode::isVisible( const Viewport & _viewPort )
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextNode::_activate()
	{
		if( SceneNode2D::_activate() == false )
		{
			return false;
		}

		m_resourceFont = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceFont>( m_resourceFontName );

		if( m_resourceFont == 0 )
		{
			return false;
		}

		m_font = m_resourceFont->getFont();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextNode::_deactivate()
	{
		SceneNode2D::_deactivate();

		Holder<ResourceManager>::hostage()
			->releaseResource( m_resourceFont );

		m_font = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextNode::loader(TiXmlElement * _xml)
	{
		SceneNode2D::loader(_xml);

		XML_FOR_EACH_TREE(_xml)
		{
			XML_CHECK_VALUE_NODE( "Font", "Name", m_resourceFont );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextNode::_render( const mt::mat3f & _rwm, const Viewport & _viewPort )
	{
		Holder<RenderEngine>::hostage()->renderText( _rwm.v2.v2 , m_font, m_message );
	}
	//////////////////////////////////////////////////////////////////////////
	void TextNode::_debugRender()
	{	
	}
	//////////////////////////////////////////////////////////////////////////
	void TextNode::setMessage( const std::string& _message )
	{
		m_message = _message;
	}
	//////////////////////////////////////////////////////////////////////////
}