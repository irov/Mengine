#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/RenderImageInterface.h"
#include "Interface/ContentInterface.h"

#include "Kernel/Rect.h"

#include "Config/Timestamp.h"

#include "math/uv4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderTextureInterface
        : public ServantInterface
    {
    public:
        virtual void release() = 0;

    public:
        virtual const RenderImageInterfacePtr & getImage() const = 0;

    public:
        virtual UniqueId getTextureId() const = 0;

    public:
        virtual Timestamp getCreateTimestamp() const = 0;

    public:
        virtual const mt::uv4f & getUV() const = 0;

        virtual void setContent( const ContentInterfacePtr & _content ) = 0;
        virtual const ContentInterfacePtr & getContent() const = 0;

    public:
        virtual uint32_t getWidth() const = 0;
        virtual uint32_t getHeight() const = 0;

    public:
        virtual float getWidthInv() const = 0;
        virtual float getHeightInv() const = 0;

    public:
        virtual bool isPow2() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderTextureInterface> RenderTextureInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
