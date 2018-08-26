#pragma once

#include "Interface/UnknownInterface.h"

#include "Kernel/IntrusivePtr.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class UnknownParticleEmitter2Interface
        : public UnknownInterface
    {
    public:
        virtual void setResourceParticle( const ResourcePtr & _resource ) = 0;
        virtual const ResourcePtr & getResourceParticle() const = 0;

        virtual void setEmitterPositionProviderOriginOffset( const mt::vec3f & _originOffset ) = 0;

        virtual void setEmitterTranslateWithParticle( bool _translateWithParticle ) = 0;
        virtual void setEmitterPositionRelative( bool _positionRelative ) = 0;
        virtual void setEmitterCameraRelative( bool _cameraRelative ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UnknownParticleEmitter2Interface> UnknownParticleEmitter2InterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}