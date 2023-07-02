#pragma once

#include "Interface/InputStreamInterface.h"
#include "Interface/DocumentInterface.h"

#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MappedInterface
        : public Mixin
    {
    public:
        virtual InputStreamInterfacePtr createInputStream( const DocumentInterfacePtr & _doc ) = 0;
        virtual bool openInputStream( const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MappedInterface> MappedInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}