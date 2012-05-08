
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
			m_images[i].texture = NULL;
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
			BIN_CASE_ATTRIBUTE( Protocol::WindowBackground_ResourceImageName, m_images[0].resourceName );
			
			BIN_CASE_ATTRIBUTE( Protocol::WindowLeftTop_ResourceImageName, m_images[1].resourceName );
			BIN_CASE_ATTRIBUTE( Protocol::WindowLeftTop_Offset, m_images[1].offset );
			
			BIN_CASE_ATTRIBUTE( Protocol::WindowTop_ResourceImageName, m_images[2].resourceName );
			BIN_CASE_ATTRIBUTE( Protocol::WindowTop_Offset, m_images[2].offset );

			BIN_CASE_ATTRIBUTE( Protocol::WindowRightTop_ResourceImageName, m_images[3].resourceName );
			BIN_CASE_ATTRIBUTE( Protocol::WindowRightTop_Offset, m_images[3].offset );

			BIN_CASE_ATTRIBUTE( Protocol::WindowRight_ResourceImageName, m_images[4].resourceName );
			BIN_CASE_ATTRIBUTE( Protocol::WindowRight_Offset, m_images[4].offset );

			BIN_CASE_ATTRIBUTE( Protocol::WindowRightBottom_ResourceImageName, m_images[5].resourceName );
			BIN_CASE_ATTRIBUTE( Protocol::WindowRightBottom_Offset, m_images[5].offset );

			BIN_CASE_ATTRIBUTE( Protocol::WindowBottom_ResourceImageName, m_images[6].resourceName );
			BIN_CASE_ATTRIBUTE( Protocol::WindowBottom_Offset, m_images[6].offset );

			BIN_CASE_ATTRIBUTE( Protocol::WindowLeftBottom_ResourceImageName, m_images[7].resourceName );
			BIN_CASE_ATTRIBUTE( Protocol::WindowLeftBottom_Offset, m_images[7].offset );

			BIN_CASE_ATTRIBUTE( Protocol::WindowLeft_ResourceImageName, m_images[8].resourceName );
			BIN_CASE_ATTRIBUTE( Protocol::WindowLeft_Offset, m_images[8].offset );
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
				continue;
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
		if( m_images[_type].resource == NULL )
		{
			return NULL;
		}
		RenderTextureInterface* texture = m_images[_type].resource->getTexture();
		return texture;
	}
	//////////////////////////////////////////////////////////////////////////
}
