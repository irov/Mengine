#pragma once

#include "Interface/SceneChangeCallbackInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DevelopmentHotkeyReloadLocaleSceneChange
        : public SceneChangeCallbackInterface
        , public Factorable
    {
    public:
        DevelopmentHotkeyReloadLocaleSceneChange();
        ~DevelopmentHotkeyReloadLocaleSceneChange() override;

    protected:
        void onSceneChange( const ScenePtr & _scene, bool _enable, bool _remove, bool _error ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevelopmentHotkeyReloadLocaleSceneChange, SceneChangeCallbackInterface> DevelopmentHotkeyReloadLocaleSceneChangePtr;
    //////////////////////////////////////////////////////////////////////////
}
