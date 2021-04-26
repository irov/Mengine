#pragma once

#include "Interface/UnknownInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/FilePath.h"
#include "Kernel/Compilable.h"
#include "Kernel/Unknowable.h"
#include "Kernel/CompilableReference.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class UnknownFEInterface
        : public UnknownInterface
    {
    public:
        virtual void setEffectName( const ConstString & _effectName ) = 0;
        virtual const ConstString & getEffectName() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    class UnknownFEFileInterface
        : virtual public UnknownFEInterface
    {
    };
    //////////////////////////////////////////////////////////////////////////
    class UnknownFECustomInterface
        : virtual public UnknownFEInterface
    {
    public:
        virtual void setFECustom( const MemoryInterfacePtr & _memory ) = 0;
        virtual const MemoryInterfacePtr & getFECustom() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}