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
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceWindow::~ResourceWindow()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceWindow::_compile()
    {
        for( uint32_t i = 0; i != ResourceWindow_Count; ++i )
        {
            WindowElement & element = m_elements[i];

            if( element.resourceImage == nullptr )
            {
                if( i == ResourceWindow_Background )
                {
                    continue;
                }

                return false;
            }

            element.resourceImage->compile();
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceWindow::_release()
    {
        for( uint32_t i = 0; i != ResourceWindow_Count; ++i )
        {
            WindowElement & element = m_elements[i];

            if( element.resourceImage != nullptr )
            {
                element.resourceImage->release();
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceWindow::setElementResourceImage( uint32_t _type, const ResourceImagePtr & _resourceImage )
    {
        MENGINE_ASSERTION( _type >= ResourceWindow_Count, "resource window '%s' set invalid type '%u' (resource)"
            , this->getName().c_str()
            , _type
        );

        WindowElement & element = m_elements[_type];

        element.resourceImage = _resourceImage;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceImagePtr & ResourceWindow::getElementResourceImage( uint32_t _type ) const
    {
        MENGINE_ASSERTION( _type >= ResourceWindow_Count, "resource window '%s' get invalid type '%u' (resource)"
            , this->getName().c_str()
            , _type
        );

        const WindowElement & element = m_elements[_type];

        const ResourceImagePtr & resourceImage = element.resourceImage;

        return resourceImage;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceWindow::setElementOffset( uint32_t _type, const mt::vec2f & _offset )
    {
        MENGINE_ASSERTION( _type >= ResourceWindow_Count, "resource window '%s' set invalid type '%u' (offset)"
            , this->getName().c_str()
            , _type
        );

        WindowElement & element = m_elements[_type];

        element.offset = _offset;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & ResourceWindow::getElementOffset( uint32_t _type ) const
    {
        MENGINE_ASSERTION( _type >= ResourceWindow_Count, "resource window '%s' get invalid type '%u' (offset)"
            , this->getName().c_str()
            , _type
        );

        const WindowElement & element = m_elements[_type];

        const mt::vec2f & offset = element.offset;

        return offset;
    }
}
