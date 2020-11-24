#pragma once

#include "Interface/Interface.h"

extern "C"
{
    struct _object;
    typedef _object PyObject;
}

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