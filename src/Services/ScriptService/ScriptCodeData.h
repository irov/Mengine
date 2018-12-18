#pragma once

#include "Interface/ScriptCodeDataInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ScriptCodeData
        : public ScriptCodeDataInterface
        , public Factorable
    {
    public:
        ScriptCodeData();
        ~ScriptCodeData() override;

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
    typedef IntrusivePtr<ScriptCodeData> ScriptCodeDataPtr;
    //////////////////////////////////////////////////////////////////////////
}