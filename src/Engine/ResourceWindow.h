#pragma once

#include "Kernel/Resource.h"

#include "Kernel/ResourceImage.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EResourceWindow
    {
        ResourceWindow_Background = 0,
        ResourceWindow_LeftTop = 1,
        ResourceWindow_Top = 2,
        ResourceWindow_RightTop = 3,
        ResourceWindow_Right = 4,
        ResourceWindow_RightBottom = 5,
        ResourceWindow_Bottom = 6,
        ResourceWindow_LeftBottom = 7,
        ResourceWindow_Left = 8,
        ResourceWindow_Count = 9,
    };
    //////////////////////////////////////////////////////////////////////////
    class ResourceWindow
        : public Resource
    {
        DECLARE_FACTORABLE( ResourceWindow );
        DECLARE_VISITABLE( Resource );

    public:
        ResourceWindow();
        ~ResourceWindow() override;

    public:
        void setElementResourceImage( uint32_t _type, const ResourceImagePtr & _resourceImage );
        const ResourceImagePtr & getElementResourceImage( uint32_t _type ) const;

        void setElementOffset( uint32_t _type, const mt::vec2f & _offset );
        const mt::vec2f & getElementOffset( uint32_t _type ) const;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        struct WindowElement
        {
            ResourceImagePtr resourceImage;
            mt::vec2f offset = {0.f, 0.f};
        };

        WindowElement m_elements[ResourceWindow_Count];
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceWindow> ResourceWindowPtr;
    //////////////////////////////////////////////////////////////////////////
}
