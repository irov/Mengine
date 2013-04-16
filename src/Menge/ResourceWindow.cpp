#	include "ResourceWindow.h"
#	include "Kernel/ResourceImplement.h"

#   include "Interface/ResourceInterface.h"
#	include "Interface/RenderSystemInterface.h"
#	include "Logger/Logger.h"

#	include "ResourceImage.h"

#	include "Metacode.h"


namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT(ResourceWindow);
	//////////////////////////////////////////////////////////////////////////
	ResourceWindow::ResourceWindow()
	{
		for( int i = 0; i < ResourceWindow_Count; i++ )
		{
			m_images[i].resource = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceWindow::~ResourceWindow()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceWindow::_loader( const Metabuf::Metadata * _meta )
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

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceWindow::_compile()
	{
		for( int i = 0; i < ResourceWindow_Count; i++ )
		{
			m_images[i].resource = NULL;

			if( m_images[i].resourceName.empty() == true )
			{
				if( i == ResourceWindow_Background )
				{
					continue;
				}
			}

			ResourceImage * resource  = RESOURCE_SERVICE(m_serviceProvider)
				->getResourceT<ResourceImage>( m_images[i].resourceName );
			
			if( resource == 0 )
			{
				LOGGER_ERROR(m_serviceProvider)( "ResourceWindow: '%s' Image resource not found resource '%s'"
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
		for( int i = 0; i < ResourceWindow_Count; i++ )
		{
			if( m_images[i].resource != NULL )
			{
				m_images[i].resource->decrementReference();
				m_images[i].resource = NULL;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceImage * ResourceWindow::getResource( int _type )
	{
		ResourceImage * resource = m_images[_type].resource;

		return resource;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceWindow::getOffset( int _type )
	{
		return m_images[_type].offset;
	}
	//////////////////////////////////////////////////////////////////////////
}
