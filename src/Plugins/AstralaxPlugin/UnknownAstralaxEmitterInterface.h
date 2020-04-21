#pragma once

#include "Interface/UnknownInterface.h"

#include "Kernel/Resource.h"
#include "Kernel/IntrusivePtr.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class UnknownAstralaxEmitterInterface
        : public UnknownInterface
    {
    public:
        virtual void setResourceAstralax( const ResourcePtr & _resource ) = 0;
        virtual const ResourcePtr & getResourceAstralax() const = 0;

        virtual void setEmitterPositionProviderOriginOffset( const mt::vec3f & _originOffset ) = 0;

        virtual void setEmitterTranslateWithParticle( bool _translateWithParticle ) = 0;
        virtual void setEmitterPositionRelative( bool _positionRelative ) = 0;
        virtual void setEmitterCameraRelative( bool _cameraRelative ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UnknownAstralaxEmitterInterface> UnknownParticleEmitterInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}