#pragma once

#include "Config/Typedef.h"
#include "Config/Char.h"

#include "Interface/Interface.h"
#include "Interface/DataInterface.h"
#include "Interface/InputStreamInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DataflowInterface
        : public Interface
    {
    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;

    public:
        virtual DataInterfacePtr create( const Char * _doc ) = 0;

    public:
        virtual bool load( const DataInterfacePtr & _data, const InputStreamInterfacePtr & _stream, const Char * _doc ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DataflowInterface> DataflowInterfacePtr;
    //////////////////////////////////////////////////////////////////////////	
}


