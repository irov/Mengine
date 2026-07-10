#pragma once

#include "Interface/DataInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/FilePath.h"

#include "Figma/Figma.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FigmaDataInterface
        : public DataInterface
    {
    public:
        virtual bool loadDocument( const MemoryInterfacePtr & _memory, const FilePath & _sourceName ) = 0;
        virtual bool loadUX( const void * _memory, size_t _size ) = 0;

    public:
        virtual bool acquire() = 0;
        virtual void release() = 0;

    public:
        virtual ::Figma::RuntimeInterface * getFigmaRuntime() const = 0;
        virtual ::Figma::DocumentInterface * getFigmaDocument() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FigmaDataInterface, DataInterface> FigmaDataInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
