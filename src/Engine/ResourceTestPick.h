#pragma once

#include "Kernel/Resource.h"
#include "Kernel/UnknownResourceImageDataInterface.h"

#include "math/vec2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceTestPick
        : public Resource
        , public UnknownResourceImageDataInterface
    {
        DECLARE_FACTORABLE( ResourceTestPick );
        DECLARE_UNKNOWABLE();
        DECLARE_VISITABLE( Resource );

    public:
        virtual bool testPoint( const mt::vec2f & _point, float _minAlpha ) const = 0;
        virtual bool testRadius( const mt::vec2f & _point, float _radius, float _minAlpha ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceTestPick> ResourceTestPickPtr;
    //////////////////////////////////////////////////////////////////////////
}
