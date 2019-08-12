#pragma once

#include "Config/Typedef.h"
#include "Config/Char.h"

#include "Interface/Interface.h"
#include "Interface/DataInterface.h"
#include "Interface/InputStreamInterface.h"
#include "Interface/MemoryInterface.h"

namespace Mengine
{
    class DataflowInterface
        : public Interface
    {
    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;

    public:
        virtual bool isThreadFlow() const = 0;

    public:
        virtual DataInterfacePtr create( const Char * _doc ) = 0;

    public:
        virtual MemoryInterfacePtr load( const InputStreamInterfacePtr & _stream, const Char * _doc ) = 0;
        virtual bool flow( const DataInterfacePtr & _data, const MemoryInterfacePtr & _memory, const Char * _doc ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DataflowInterface> DataflowInterfacePtr;
    //////////////////////////////////////////////////////////////////////////	
}