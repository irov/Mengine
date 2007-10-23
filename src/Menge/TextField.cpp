#	include "TextField.h" 

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
	OBJECT_IMPLEMENT(TextField);
	//////////////////////////////////////////////////////////////////////////
	TextField::TextField()
	{}
	//////////////////////////////////////////////////////////////////////////
	TextField::~TextField()
	{}
	///////////////////////////////////////////////////////////////////////////
	bool TextField::isVisible( const Viewport & _viewPort )
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextField::_activate()
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
	void TextField::_deactivate()
	{
		SceneNode2D::_deactivate();

		Holder<ResourceManager>::hostage()
			->releaseResource( m_resourceFont );

		m_font = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::loader(TiXmlElement * _xml)
	{
		SceneNode2D::loader(_xml);

		XML_FOR_EACH_TREE(_xml)
		{
			XML_CHECK_VALUE_NODE( "Font", "Name", m_resourceFontName );
			XML_CHECK_VALUE_NODE( "Text", "Value", m_message);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_render( const mt::mat3f & _rwm, const Viewport & _viewPort )
	{
		Holder<RenderEngine>::hostage()->renderText( m_font, m_message, _rwm.m );
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_debugRender()
	{	
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setMessage( const std::string& _message )
	{
		m_message = _message;
	}
	//////////////////////////////////////////////////////////////////////////
}