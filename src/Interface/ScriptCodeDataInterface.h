#pragma once

#include "Interface/DataInterface.h"

#include "Environment/Python/PythonIncluder.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ScriptCodeDataInterface
        : public DataInterface
    {
    public:
        virtual const pybind::object & getScriptCode() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ScriptCodeDataInterface, DataInterface> ScriptCodeDataInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}