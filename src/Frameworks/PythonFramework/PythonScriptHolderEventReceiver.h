#pragma once

#include "ScriptHolder.h"

#include "Environment/Python/PythonEventReceiver.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class PythonScriptHolderEventReceiver
        : public PythonEventReceiver
        , public ScriptHolderEventReceiverInterface
        , public Factorable
    {
    public:
        PythonScriptHolderEventReceiver();
        ~PythonScriptHolderEventReceiver() override;

    public:
        pybind::object onScriptHolderKeepScript() override;
        void onScriptHolderReleaseScript( const pybind::object & _script ) override;
    };
}
