#pragma once

#include "Interface/ScriptModuleInterface.h"
#include "Interface/LoggerInterface.h"

#include "pybind/types.hpp"
#include "pybind/pybind.hpp"

namespace Mengine
{
    class PythonScriptModule
        : public ScriptModuleInterface
        , public Factorable
    {
    public:
        PythonScriptModule();
        ~PythonScriptModule() override;

    public:
        bool initialize( const pybind::module & _module );

    protected:
        bool onInitialize( const ConstString & _method ) override;
        bool onFinalize( const ConstString & _method ) override;

    protected:
        const pybind::module & getModule() const override;

    protected:
        pybind::module m_module;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonScriptModule> ScriptModulePtr;
    //////////////////////////////////////////////////////////////////////////
}