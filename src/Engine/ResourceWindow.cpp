#include "ResourceWindow.h"

#include "Interface/ResourceServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceWindow::ResourceWindow()
    {
        for( uint32_t i = 0; i < ResourceWindow_Count; i++ )
        {
            m_elements[i].resourceImage = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceWindow::~ResourceWindow()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceWindow::_compile()
    {
        for( uint32_t i = 0; i < ResourceWindow_Count; i++ )
        {
            WindowElement & element = m_elements[i];

            element.resourceImage = nullptr;

            if( element.resourceImageName.empty() == true )
            {
                if( i == ResourceWindow_Background )
                {
                    continue;
                }
            }

            const ResourceImagePtr & resourceImage = RESOURCE_SERVICE()
                ->getResource( m_elements[i].resourceImageName );

            MENGINE_ASSERTION_MEMORY_PANIC( resourceImage, false, "window '%s' not found resource '%s'"
                , this->getName().c_str()
                , element.resourceImageName.c_str()
            );

            element.resourceImage = resourceImage;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceWindow::_release()
    {
        for( uint32_t i = 0; i < ResourceWindow_Count; i++ )
        {
            WindowElement & element = m_elements[i];

            if( element.resourceImage != nullptr )
            {
                element.resourceImage->release();
                element.resourceImage = nullptr;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceWindow::setElementResourceImageName( uint32_t _type, const ConstString & _resourceImageName )
    {
        MENGINE_ASSERTION( _type >= ResourceWindow_Count, "resource window '%s' set invalid type '%d' (resource)"
            , this->getName().c_str()
            , _type
        );

        WindowElement & element = m_elements[_type];

        element.resourceImageName = _resourceImageName;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & ResourceWindow::getElementResourceImageName( uint32_t _type ) const
    {
        MENGINE_ASSERTION( _type >= ResourceWindow_Count, "resource window '%s' get invalid type '%d' (resource)"
            , this->getName().c_str()
            , _type
        );

        const WindowElement & element = m_elements[_type];

        const ConstString & resourceImageName = element.resourceImageName;

        return resourceImageName;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceWindow::setElementOffset( uint32_t _type, const mt::vec2f & _offset )
    {
        MENGINE_ASSERTION( _type >= ResourceWindow_Count, "resource window '%s' set invalid type '%d' (offset)"
            , this->getName().c_str()
            , _type
        );

        WindowElement & element = m_elements[_type];

        element.offset = _offset;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & ResourceWindow::getElementOffset( uint32_t _type ) const
    {
        MENGINE_ASSERTION( _type >= ResourceWindow_Count, "resource window '%s' get invalid type '%d' (offset)"
            , this->getName().c_str()
            , _type
        );

        const WindowElement & element = m_elements[_type];

        const mt::vec2f & offset = element.offset;

        return offset;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceImagePtr & ResourceWindow::getElementResourceImage( uint32_t _type ) const
    {
        MENGINE_ASSERTION( _type >= ResourceWindow_Count, "resource window '%s' get invalid type '%d' (image)"
            , this->getName().c_str()
            , _type
        );

        const WindowElement & element = m_elements[_type];

        const ResourceImagePtr & resourceImage = element.resourceImage;

        return resourceImage;
    }
    //////////////////////////////////////////////////////////////////////////
}
