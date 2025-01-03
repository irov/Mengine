#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/Scene.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SceneChangeCallbackInterface
        : public Mixin
    {
    public:
        virtual void onSceneChange( const ScenePtr & _scene, bool _enable, bool _remove, bool _error ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SceneChangeCallbackInterface> SceneChangeCallbackInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}