#pragma once

#include "Kernel/Mixin.h"

#include "math/mat4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderCameraInterface
        : public Mixin
    {
    public:
        virtual void fromWorldToScreenPosition( const mt::mat4f & _worldMatrix, mt::vec2f * const _screenPosition ) const = 0;
        
    public:
        virtual void fromScreenToWorldPosition( const mt::vec2f & _screenPoint, float _deep, mt::vec3f * const _worldPosition ) const = 0;
        virtual void fromScreenToWorldDelta( const mt::vec2f & _screenDelta, float _deep, mt::vec3f * const _worldDelta ) const = 0;

    public:
        virtual const mt::mat4f & getCameraViewMatrix() const = 0;
        virtual const mt::mat4f & getCameraViewMatrixInv() const = 0;

        virtual const mt::mat4f & getCameraProjectionMatrix() const = 0;
        virtual const mt::mat4f & getCameraProjectionMatrixInv() const = 0;

    public:
        virtual const mt::mat4f & getCameraViewProjectionMatrix() const = 0;
        virtual const mt::mat4f & getCameraViewProjectionMatrixInv() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderCameraInterface> RenderCameraInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}