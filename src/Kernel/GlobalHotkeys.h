#pragma once

#include "Kernel/GlobalInputHandlerHelper.h"
#include "Kernel/Vector.h"

#include "Config/UniqueId.h"

namespace Mengine
{
    class GlobalHotkeys
    {
    public:
        GlobalHotkeys();
        ~GlobalHotkeys();

    public:
        void addHotkey( EKeyCode _code, const Helper::LambdaInputKeyEvent & _event, const DocumentInterfacePtr & _doc );
        void removeHotkeys();

    protected:
        typedef Vector<UniqueId> VectorHotkeys;
        VectorHotkeys m_hotkeys;
    };
}
