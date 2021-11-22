#pragma once

#include "TexturepackerInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceMultiTexturepacker
        : public Resource
        , public UnknownResourceMultiTexturepackerInterface
    {
        DECLARE_FACTORABLE( ResourceMultiTexturepacker );
        DECLARE_VISITABLE( Resource );
        DECLARE_UNKNOWABLE();

    public:
        ResourceMultiTexturepacker();
        ~ResourceMultiTexturepacker() override;

    public:
        void addResourceTexturepacker( const ResourcePtr & _resourceTexturepacker ) override;
        const VectorResources & getResourceTexturepackers() const override;

    public:
        bool findFrame( const ConstString & _name, ResourceImagePtr * const _resourceImage ) const override;

    public:
        bool foreachFrames( const LambdaFrames & _lambdaFrames ) const override;

    public:
        void setStripFrameNameExtension( bool _value ) override;
        bool getStripFrameNameExtension() const override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        VectorResources m_resourceTexturepackers;

        bool m_needStripFrameNameExtension;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceMultiTexturepacker> ResourceMultiTexturepackerPtr;
    //////////////////////////////////////////////////////////////////////////
}