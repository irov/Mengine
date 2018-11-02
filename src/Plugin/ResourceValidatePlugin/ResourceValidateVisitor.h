#pragma once

#include "Engine/ResourceImageDefault.h"
#include "Engine/ResourceImageData.h"
#include "Engine/ResourceImageSequence.h"
#include "Engine/ResourceMusic.h"
#include "Engine/ResourceSound.h"
#include "Engine/ResourceHIT.h"
#include "Engine/ResourceFile.h"

#include "Kernel/Visitor.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceValidateVisitor
        : public Visitor
        , public Factorable
        , public ConcreteVisitorR<ResourceImageDefault, bool>
        , public ConcreteVisitorR<ResourceImageData, bool>
        , public ConcreteVisitorR<ResourceImageSequence, bool>
        , public ConcreteVisitorR<ResourceMusic, bool>
        , public ConcreteVisitorR<ResourceSound, bool>
        , public ConcreteVisitorR<ResourceHIT, bool>
        , public ConcreteVisitorR<ResourceFile, bool>
    {
    public:
        ResourceValidateVisitor();
        ~ResourceValidateVisitor() override;

    protected:
        bool accept( ResourceImageDefault * _resource ) override;
        bool accept( ResourceImageData * _resource ) override;
        bool accept( ResourceImageSequence * _resource ) override;
        bool accept( ResourceMusic * _resource ) override;
        bool accept( ResourceSound * _resource ) override;
        bool accept( ResourceHIT * _resource ) override;
        bool accept( ResourceFile * _resource ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ResourceValidateVisitor> ResourceValidateVisitorPtr;
    //////////////////////////////////////////////////////////////////////////
}