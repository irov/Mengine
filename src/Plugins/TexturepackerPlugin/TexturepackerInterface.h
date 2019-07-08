#pragma once

#include "Interface/UnknownInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/VectorResourceImages.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class UnknownResourceTexturepackerInterface
        : public UnknownInterface
    {
    public:
        virtual uint32_t getAtlasWidth() const = 0;
        virtual uint32_t getAtlasHeight() const = 0;

        virtual float getAtlasWidthInv() const = 0;
        virtual float getAtlasHeightInv() const = 0;

    public:
        virtual const ResourceImagePtr & getFrame( const ConstString & _name ) const = 0;

    public:
        virtual const VectorResourceImages & getFrames() const = 0;
    };
}