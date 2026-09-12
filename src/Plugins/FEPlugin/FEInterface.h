#pragma once

#include "Interface/UnknownInterface.h"
#include "Interface/FontEffectInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/FilePath.h"
#include "Kernel/Compilable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class UnknownFEInterface
        : public UnknownFontEffectFileInterface
    {
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