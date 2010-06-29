
#	include "ResourceWindow.h"

#	include "ResourceImplement.h"
#	include "XmlEngine.h"
#	include "Texture.h"
#	include "RenderEngine.h"
#	include "ResourceImage.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT(ResourceWindow);
	//////////////////////////////////////////////////////////////////////////
	ResourceWindow::ResourceWindow()
	{
		for( int i = 0; i < MAX_WINDOW_ELEMENTS; i++ )
		{
			m_images[i].image = NULL;
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
			XML_CASE_ATTRIBUTE_NODE( "Background", "Image", m_images[0].path );
			XML_CASE_ATTRIBUTE_NODE( "Background", "Codec", m_images[0].codec );

			XML_CASE_ATTRIBUTE_NODE( "LeftTop", "Image", m_images[1].path );
			XML_CASE_ATTRIBUTE_NODE( "LeftTop", "Codec", m_images[1].codec );

			XML_CASE_ATTRIBUTE_NODE( "Top", "Image", m_images[2].path );
			XML_CASE_ATTRIBUTE_NODE( "Top", "Codec", m_images[2].codec );

			XML_CASE_ATTRIBUTE_NODE( "RightTop", "Image", m_images[3].path );
			XML_CASE_ATTRIBUTE_NODE( "RightTop", "Codec", m_images[3].codec );

			XML_CASE_ATTRIBUTE_NODE( "Right", "Image", m_images[4].path );
			XML_CASE_ATTRIBUTE_NODE( "Right", "Codec", m_images[4].codec );

			XML_CASE_ATTRIBUTE_NODE( "RightBottom", "Image", m_images[5].path );
			XML_CASE_ATTRIBUTE_NODE( "RightBottom", "Codec", m_images[5].codec );

			XML_CASE_ATTRIBUTE_NODE( "Bottom", "Image", m_images[6].path );
			XML_CASE_ATTRIBUTE_NODE( "Bottom", "Codec", m_images[6].codec );

			XML_CASE_ATTRIBUTE_NODE( "LeftBottom", "Image", m_images[7].path );
			XML_CASE_ATTRIBUTE_NODE( "LeftBottom", "Codec", m_images[7].codec );

			XML_CASE_ATTRIBUTE_NODE( "Left", "Image", m_images[8].path );
			XML_CASE_ATTRIBUTE_NODE( "Left", "Codec", m_images[8].codec );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceWindow::_compile()
	{
		for( int i = 0; i < MAX_WINDOW_ELEMENTS; i++ )
		{
			if( m_images[i].path.empty() == true )
			{
				continue;
			}

			const ConstString & category = this->getCategory();

			if( m_images[i].codec.empty() == true )
			{
				m_images[i].codec
					= ResourceImage::s_getImageCodec( m_images[i].path.str() );
			}

			m_images[i].image = RenderEngine::get()
				->loadTexture( category, m_images[i].path, m_images[i].codec );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceWindow::_release()
	{
		for( int i = 0; i < MAX_WINDOW_ELEMENTS; i++ )
		{
			if( m_images[i].image != NULL )
			{
				Holder<RenderEngine>::get()
					->releaseTexture( m_images[i].image );
				m_images[i].image = NULL;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Texture* ResourceWindow::getImage( int _type )
	{
		return m_images[_type].image;
	}
	//////////////////////////////////////////////////////////////////////////
}
