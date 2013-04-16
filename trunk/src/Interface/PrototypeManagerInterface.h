#   pragma once

#   include "Interface/ScriptSystemInterface.h"

#   include "Core/ConstString.h"

namespace Menge
{
    class PrototypeManagerInterface
		: virtual public ServiceInterface
    {
    public:
        virtual bool addPrototype( const ConstString & _prototype, PyObject * _module ) = 0;
        virtual PyObject * getPrototype( const ConstString & _prototype ) const = 0;

    public:
        virtual bool hasPrototype( const ConstString & _prototype, PyObject ** _module ) const = 0;
    };
}

