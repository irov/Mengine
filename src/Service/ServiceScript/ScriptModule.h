#pragma once

#include "Interface/ScriptSystemInterface.h"
#include "Interface/LoggerInterface.h"

#include "pybind/types.hpp"
#include "pybind/pybind.hpp"

namespace Mengine
{
    class ScriptModule
        : public ScriptModuleInterface
        , public Factorable
    {
    public:
        ScriptModule();
        ~ScriptModule() override;

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
    typedef IntrusivePtr<ScriptModule> ScriptModulePtr;
}