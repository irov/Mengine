#pragma once

#include "Interface/ScriptCodeDataInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PythonScriptCodeData
        : public ScriptCodeDataInterface
    {
        DECLARE_FACTORABLE( PythonScriptCodeData );

    public:
        PythonScriptCodeData();
        ~PythonScriptCodeData() override;

    public:
        bool acquire() override;
        void release() override;

    public:
        void setScriptCode( const pybind::object & _moduleCode );
        const pybind::object & getScriptCode() const override;

    protected:
        pybind::object m_moduleCode;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonScriptCodeData> ScriptCodeDataPtr;
    //////////////////////////////////////////////////////////////////////////
}