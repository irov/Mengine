
#	include "ResourceWindow.h"

#	include "ResourceImplement.h"
#	include "XmlEngine.h"
#	include "Texture.h"
#	include "RenderEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT(ResourceWindow);
	//////////////////////////////////////////////////////////////////////////
	ResourceWindow::ResourceWindow()
	{
		for( int i = 0; i < MAX_WINDOW_ELEMENTS; i++ )
		{
			m_renderImage[i] = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceWindow::~ResourceWindow()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceWindow::loader( XmlElement * _xml )
	{
		ResourceReference::loader( _xml );

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "Background", "Image", m_imagePath[0] );
			XML_CASE_ATTRIBUTE_NODE( "LeftTop", "Image", m_imagePath[1] );
			XML_CASE_ATTRIBUTE_NODE( "Top", "Image", m_imagePath[2] );
			XML_CASE_ATTRIBUTE_NODE( "RightTop", "Image", m_imagePath[3] );
			XML_CASE_ATTRIBUTE_NODE( "Right", "Image", m_imagePath[4] );
			XML_CASE_ATTRIBUTE_NODE( "RightBottom", "Image", m_imagePath[5] );
			XML_CASE_ATTRIBUTE_NODE( "Bottom", "Image", m_imagePath[6] );
			XML_CASE_ATTRIBUTE_NODE( "LeftBottom", "Image", m_imagePath[7] );
			XML_CASE_ATTRIBUTE_NODE( "Left", "Image", m_imagePath[8] );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const String & ResourceWindow::getBackgroundImage() const
	{
		return m_imagePath[0];
	}
	//////////////////////////////////////////////////////////////////////////
	const String & ResourceWindow::getLeftTopImage() const
	{
		return m_imagePath[1];
	}
	//////////////////////////////////////////////////////////////////////////
	const String & ResourceWindow::getTopImage() const
	{
		return m_imagePath[2];
	}
	//////////////////////////////////////////////////////////////////////////
	const String & ResourceWindow::getRightTopImage() const
	{
		return m_imagePath[3];
	}
	//////////////////////////////////////////////////////////////////////////
	const String & ResourceWindow::getRightImage() const
	{
		return m_imagePath[4];
	}
	//////////////////////////////////////////////////////////////////////////
	const String & ResourceWindow::getRightBottomImage() const
	{
		return m_imagePath[5];
	}
	//////////////////////////////////////////////////////////////////////////
	const String & ResourceWindow::getBottomImage() const
	{
		return m_imagePath[6];
	}
	//////////////////////////////////////////////////////////////////////////
	const String & ResourceWindow::getLeftBottomImage() const
	{
		return m_imagePath[7];
	}
	//////////////////////////////////////////////////////////////////////////
	const String & ResourceWindow::getLeftImage() const
	{
		return m_imagePath[8];
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceWindow::_compile()
	{
		for( int i = 0; i < MAX_WINDOW_ELEMENTS; i++ )
		{
			if( m_imagePath[i].empty() == true )
			{
				continue;
			}
			m_renderImage[i] = Holder<RenderEngine>::hostage()
								->loadTexture( m_params.category, m_imagePath[i] );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceWindow::_release()
	{
		for( int i = 0; i < MAX_WINDOW_ELEMENTS; i++ )
		{
			if( m_renderImage[i] != NULL )
			{
				Holder<RenderEngine>::hostage()
					->releaseTexture( m_renderImage[i] );
				m_renderImage[i] = NULL;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Texture* ResourceWindow::getImage( int _type )
	{
		return m_renderImage[_type];
	}
	//////////////////////////////////////////////////////////////////////////
}
