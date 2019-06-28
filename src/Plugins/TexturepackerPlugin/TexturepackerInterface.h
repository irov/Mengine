#pragma once

#include "Interface/UnknownInterface.h"

#include "Kernel/ResourceImage.h"
#include "Kernel/ConstString.h"

namespace Mengine
{
    class UnknownResourceTexturepackerInterface
        : public UnknownInterface
    {
    public:
        virtual const ResourceImagePtr& getFrame(const ConstString& _name) const = 0;
    };
}