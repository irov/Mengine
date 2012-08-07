#	include "ResourceWindow.h"

#	include "ResourceManager.h"
#	include "ResourceImplement.h"

#	include "RenderEngine.h"
#	include "LogEngine.h"

#	include "ResourceImage.h"

#	include "Metacode.h"


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
	void ResourceWindow::loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceWindow * metadata 
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceWindow *>(_meta);

        metadata->swap_WindowBackground_ResourceImageName( m_images[ResourceWindow_Background].resourceName );

        metadata->swap_WindowBottom_ResourceImageName( m_images[ResourceWindow_Bottom].resourceName );
        metadata->swap_WindowLeft_ResourceImageName( m_images[ResourceWindow_Left].resourceName );
        metadata->swap_WindowLeftBottom_ResourceImageName( m_images[ResourceWindow_LeftBottom].resourceName );
        metadata->swap_WindowLeftTop_ResourceImageName( m_images[ResourceWindow_LeftTop].resourceName );
        metadata->swap_WindowRight_ResourceImageName( m_images[ResourceWindow_Right].resourceName );
        metadata->swap_WindowRightBottom_ResourceImageName( m_images[ResourceWindow_RightBottom].resourceName );
        metadata->swap_WindowRightTop_ResourceImageName( m_images[ResourceWindow_RightTop].resourceName );
        metadata->swap_WindowTop_ResourceImageName( m_images[ResourceWindow_Top].resourceName );
        
        m_images[ResourceWindow_Bottom].offset = metadata->get_WindowBottom_Offset();
        m_images[ResourceWindow_Left].offset = metadata->get_WindowLeft_Offset();
        m_images[ResourceWindow_LeftBottom].offset = metadata->get_WindowLeftBottom_Offset();
        m_images[ResourceWindow_LeftTop].offset = metadata->get_WindowLeftTop_Offset();
        m_images[ResourceWindow_Right].offset = metadata->get_WindowRight_Offset();
        m_images[ResourceWindow_RightBottom].offset = metadata->get_WindowRightBottom_Offset();
        m_images[ResourceWindow_RightTop].offset = metadata->get_WindowRightTop_Offset();
        m_images[ResourceWindow_Top].offset = metadata->get_WindowTop_Offset();
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
