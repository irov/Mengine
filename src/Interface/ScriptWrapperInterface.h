#pragma once

#include "Environment/Python/PythonForwardDeclaration.h"

#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Scriptable;
    //////////////////////////////////////////////////////////////////////////
    class ScriptWrapperInterface
        : public Mixin
    {
    public:
        virtual PyObject * wrap( Scriptable * _scriptable, PyObject * _embed ) = 0;
        virtual void unwrap( PyObject * _obj ) = 0;

    public:
        virtual bool isWrap( PyObject * _obj ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ScriptWrapperInterface> ScriptWrapperInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}