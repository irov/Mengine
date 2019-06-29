#pragma once

#include "Interface/ScriptCodeDataInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PythonScriptCodeData
        : public ScriptCodeDataInterface
        , public Factorable
    {
    public:
        PythonScriptCodeData();
        ~PythonScriptCodeData() override;

    public:
        bool acquire() override;
        void release() override;

    protected:
        Pointer allocateMemory( size_t _size ) const override;

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