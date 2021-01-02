#pragma once

#include "Interface/Interface.h"

#include "Environment/Python/PythonForwardDeclaration.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Scriptable;
    //////////////////////////////////////////////////////////////////////////
    class ScriptWrapperInterface
        : public Interface
    {
    public:
        virtual PyObject * wrap( Scriptable * _scriptable ) = 0;
        virtual void unwrap( PyObject * _obj ) = 0;

    public:
        virtual bool isWrap( PyObject * _obj ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ScriptWrapperInterface> ScriptWrapperInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}