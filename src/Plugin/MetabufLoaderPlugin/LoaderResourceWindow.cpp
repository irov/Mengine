#include "ResourceWindow.h"

#include "Interface/ResourceServiceInterface.h"

#include "Metacode/Metacode.h"

#include "Kernel/Logger.h"

#include "Kernel/ResourceImage.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceWindow::ResourceWindow()
    {
        for( int i = 0; i < ResourceWindow_Count; i++ )
        {
            m_images[i].resource = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceWindow::~ResourceWindow()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceWindow::_loader( const Metabuf::Metadata * _meta )
    {
        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceWindow * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceWindow *>(_meta);

        metadata->get_WindowBackground_ResourceImageName( &m_images[ResourceWindow_Background].resourceName );

        m_images[ResourceWindow_Bottom].resourceName = metadata->get_WindowBottom_ResourceImageName();
        m_images[ResourceWindow_Left].resourceName = metadata->get_WindowLeft_ResourceImageName();
        m_images[ResourceWindow_LeftBottom].resourceName = metadata->get_WindowLeftBottom_ResourceImageName();
        m_images[ResourceWindow_LeftTop].resourceName = metadata->get_WindowLeftTop_ResourceImageName();
        m_images[ResourceWindow_Right].resourceName = metadata->get_WindowRight_ResourceImageName();
        m_images[ResourceWindow_RightBottom].resourceName = metadata->get_WindowRightBottom_ResourceImageName();
        m_images[ResourceWindow_RightTop].resourceName = metadata->get_WindowRightTop_ResourceImageName();
        m_images[ResourceWindow_Top].resourceName = metadata->get_WindowTop_ResourceImageName();

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
            m_images[i].resource = nullptr;

            if( m_images[i].resourceName.empty() == true )
            {
                if( i == ResourceWindow_Background )
                {
                    continue;
                }
            }

            ResourceImagePtr resource = RESOURCE_SERVICE()
                ->getResource( m_images[i].resourceName );

            if( resource == 0 )
            {
                LOGGER_ERROR( "ResourceWindow: '%s' Image resource not found resource '%s'"
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
            if( m_images[i].resource != nullptr )
            {
                m_images[i].resource->decrementReference();
                m_images[i].resource = nullptr;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceImagePtr & ResourceWindow::getResource( int _type ) const
    {
        const ResourceImagePtr & resource = m_images[_type].resource;

        return resource;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & ResourceWindow::getOffset( int _type ) const
    {
        const mt::vec2f & offset = m_images[_type].offset;

        return offset;
    }
    //////////////////////////////////////////////////////////////////////////
}
