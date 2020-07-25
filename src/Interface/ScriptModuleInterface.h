#pragma once

#include "Interface/Interface.h"

#include "Kernel/ConstString.h"

namespace pybind
{
    class module;
}

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ScriptModuleInterface
        : public Interface
    {
    public:
        virtual bool onInitialize( const ConstString & _method ) = 0;
        virtual bool onFinalize( const ConstString & _method ) = 0;

    public:
        virtual const pybind::module & getModule() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ScriptModuleInterface> ScriptModuleInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}