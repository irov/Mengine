#pragma once

#include "Interface/UnknownInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/VectorResources.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/VectorResourceImages.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class UnknownResourceTexturepackerGetterInterface
        : public UnknownInterface
    {
    public:
        virtual bool findFrame( const ConstString & _name, ResourceImagePtr * const _resourceImage ) const = 0;

    public:
        typedef Lambda<bool( const ResourceImagePtr & )> LambdaFrames;
        virtual bool foreachFrames( const LambdaFrames & _lambdaFrames ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    class UnknownResourceTexturepackerInterface
        : public UnknownResourceTexturepackerGetterInterface
    {
    public:
        virtual uint32_t getAtlasWidth() const = 0;
        virtual uint32_t getAtlasHeight() const = 0;

        virtual float getAtlasWidthInv() const = 0;
        virtual float getAtlasHeightInv() const = 0;

    public:
        virtual const ResourceImagePtr & getAtlasImage() const = 0;

    public:
        virtual void setResourceJSON( const ResourcePtr & _resourceJSON ) = 0;
        virtual const ResourcePtr & getResourceJSON() const = 0;
        
    public:
        virtual void setResourceImage( const ResourceImagePtr & _resourceImage ) = 0;
        virtual const ResourceImagePtr & getResourceImage() const = 0;

    public:
        // means that from each frame name will be stripped extension (i.e. "some_frame.png" -> "some_frame")
        virtual void setStripFrameNameExtension( bool _value ) = 0;
        virtual bool getStripFrameNameExtension() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    class UnknownResourceMultiTexturepackerInterface
        : public UnknownResourceTexturepackerGetterInterface
    {
    public:
        virtual void addResourceTexturepacker( const ResourcePtr & _resourceJSONName ) = 0;
        virtual const VectorResources & getResourceTexturepackers() const = 0;

    public:
        virtual void setStripFrameNameExtension( bool _value ) = 0;
        virtual bool getStripFrameNameExtension() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}