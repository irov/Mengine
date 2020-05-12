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
        virtual bool hasFrame( const ConstString & _name, ResourceImagePtr * _resourceImage ) const = 0;

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

    public:
        // means that from each frame name will be stripped extension (i.e. "some_frame.png" -> "some_frame")
        virtual void setStripFrameNameExtension( bool _value ) = 0;
        virtual bool getStripFrameNameExtension() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    class UnknownResourceMultiTexturepackerInterface
        : public UnknownInterface
    {
    public:
        virtual void addResourceTexturepackerName( const ConstString & _resourceJSONName ) = 0;
        virtual const VectorConstString & getResourceTexturepackerNames() const = 0;

    public:
        virtual bool findFrame( const ConstString & _name, ResourceImagePtr * _resourceImage ) const = 0;
    };
}