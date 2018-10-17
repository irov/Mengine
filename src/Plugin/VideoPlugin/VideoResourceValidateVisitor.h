#pragma once

#include "ResourceVideo.h"

#include "Kernel/Visitor.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class VideoResourceValidateVisitor
        : public Visitor
        , public Factorable
        , public ConcreteVisitorR<ResourceVideo, bool>
    {
    public:
		VideoResourceValidateVisitor();
        ~VideoResourceValidateVisitor() override;

    protected:
        bool accept( ResourceVideo * _resource ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<VideoResourceValidateVisitor> VideoResourceValidateVisitorPtr;
    //////////////////////////////////////////////////////////////////////////
}