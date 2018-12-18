#pragma once

#include "Interface/DataInterface.h"

#include "pybind/object.hpp"

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