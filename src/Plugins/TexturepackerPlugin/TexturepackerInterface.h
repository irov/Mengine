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
        virtual const ResourceImagePtr & getAtlasImage() const = 0;

    public:
        virtual const ResourceImagePtr & getFrame( const ConstString & _name ) const = 0;

    public:
        virtual const VectorResourceImages & getFrames() const = 0;

    public:
        virtual void setResourceJSONName( const ConstString & _resourceJSONName ) = 0;
        virtual const ConstString & getResourceJSONName() const = 0;
        

    public:
        virtual void setResourceImageName( const ConstString & _resourceImageName ) = 0;
        virtual const ConstString & getResourceImageName() const = 0;
    };
}