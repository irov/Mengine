#pragma once

#include "Interface/ScriptModuleInterface.h"
#include "Interface/LoggerInterface.h"

#include "Environment/Python/PythonIncluder.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PythonScriptModule
        : public ScriptModuleInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( PythonScriptModule );

    public:
        PythonScriptModule();
        ~PythonScriptModule() override;

    public:
        bool initialize( const pybind::module & _module );
        void finalize();

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