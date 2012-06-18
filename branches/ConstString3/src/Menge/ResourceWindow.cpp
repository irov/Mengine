
#	include "ResourceWindow.h"
#	include "ResourceManager.h"
#	include "ResourceImplement.h"

#	include "RenderEngine.h"
#	include "ResourceImage.h"

#	include "BinParser.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT(ResourceWindow);
	//////////////////////////////////////////////////////////////////////////
	ResourceWindow::ResourceWindow()
	{
		for( int i = 0; i < MAX_WINDOW_ELEMENTS; i++ )
		{
			m_images[i].resource = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceWindow::~ResourceWindow()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceWindow::loader( BinParser * _parser )
	{
		ResourceReference::loader( _parser );

		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE( Protocol::WindowBackground_ResourceImageName, m_images[ResourceWindow_Background].resourceName );
			
			BIN_CASE_ATTRIBUTE( Protocol::WindowLeftTop_ResourceImageName, m_images[ResourceWindow_LeftTop].resourceName );
			BIN_CASE_ATTRIBUTE( Protocol::WindowLeftTop_Offset, m_images[ResourceWindow_LeftTop].offset );
			
			BIN_CASE_ATTRIBUTE( Protocol::WindowTop_ResourceImageName, m_images[ResourceWindow_Top].resourceName );
			BIN_CASE_ATTRIBUTE( Protocol::WindowTop_Offset, m_images[ResourceWindow_Top].offset );

			BIN_CASE_ATTRIBUTE( Protocol::WindowRightTop_ResourceImageName, m_images[ResourceWindow_RightTop].resourceName );
			BIN_CASE_ATTRIBUTE( Protocol::WindowRightTop_Offset, m_images[ResourceWindow_RightTop].offset );

			BIN_CASE_ATTRIBUTE( Protocol::WindowRight_ResourceImageName, m_images[ResourceWindow_Right].resourceName );
			BIN_CASE_ATTRIBUTE( Protocol::WindowRight_Offset, m_images[ResourceWindow_Right].offset );

			BIN_CASE_ATTRIBUTE( Protocol::WindowRightBottom_ResourceImageName, m_images[ResourceWindow_RightBottom].resourceName );
			BIN_CASE_ATTRIBUTE( Protocol::WindowRightBottom_Offset, m_images[ResourceWindow_RightBottom].offset );

			BIN_CASE_ATTRIBUTE( Protocol::WindowBottom_ResourceImageName, m_images[ResourceWindow_Bottom].resourceName );
			BIN_CASE_ATTRIBUTE( Protocol::WindowBottom_Offset, m_images[ResourceWindow_Bottom].offset );

			BIN_CASE_ATTRIBUTE( Protocol::WindowLeftBottom_ResourceImageName, m_images[ResourceWindow_LeftBottom].resourceName );
			BIN_CASE_ATTRIBUTE( Protocol::WindowLeftBottom_Offset, m_images[ResourceWindow_LeftBottom].offset );

			BIN_CASE_ATTRIBUTE( Protocol::WindowLeft_ResourceImageName, m_images[ResourceWindow_Left].resourceName );
			BIN_CASE_ATTRIBUTE( Protocol::WindowLeft_Offset, m_images[ResourceWindow_Left].offset );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceWindow::_compile()
	{
		for( int i = 0; i < MAX_WINDOW_ELEMENTS; i++ )
		{
			m_images[i].resource = NULL;

			if( m_images[i].resourceName.empty() == true )
			{
				if( i == ResourceWindow_Background )
				{
					continue;
				}
			}

			ResourceImage * resource  = ResourceManager::get()
				->getResourceT<ResourceImage>( m_images[i].resourceName );
			
			if( resource == 0 )
			{
				MENGE_LOG_ERROR( "ResourceWindow: '%s' Image resource not found resource '%s'"
					, m_name.c_str()
					, m_images[i].resourceName.c_str() 
					);

				return false;
			}

			m_images[i].resource = resource;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceWindow::_release()
	{
		for( int i = 0; i < MAX_WINDOW_ELEMENTS; i++ )
		{
			if( m_images[i].resource != NULL )
			{
				m_images[i].resource->decrementReference();
				m_images[i].resource = NULL;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	RenderTextureInterface* ResourceWindow::getImage( int _type )
	{
		ResourceImage * resource = m_images[_type].resource;
		if( resource == NULL )
		{
			return NULL;
		}
			
		RenderTextureInterface* texture = resource->getTexture();
		return texture;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceWindow::getOffset( int _type )
	{
		return m_images[_type].offset;
	}
	//////////////////////////////////////////////////////////////////////////
}
