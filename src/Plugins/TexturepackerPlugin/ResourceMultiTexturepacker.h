#pragma once

#include "TexturepackerInterface.h"

#include "Kernel/Resource.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceMultiTexturepacker
        : public Resource
        , public UnknownResourceMultiTexturepackerInterface
    {
        DECLARE_VISITABLE( Resource );
        DECLARE_UNKNOWABLE();

    public:
        ResourceMultiTexturepacker();
        ~ResourceMultiTexturepacker() override;

    public:
        void addResourceTexturepackerName( const ConstString & _resourceJSONName ) override;
        const VectorConstString & getResourceTexturepackerNames() const override;

    public:
        bool findFrame( const ConstString & _name, ResourceImagePtr * const _resourceImage ) const override;

    public:
        void setStripFrameNameExtension( bool _value ) override;
        bool getStripFrameNameExtension() const override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        VectorConstString m_resourceTexturepackerNames;

        typedef Vector<ResourcePtr> VectorResourceTexturepackers;
        VectorResourceTexturepackers m_resourceTexturepackers;

        bool m_needStripFrameNameExtension;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceMultiTexturepacker> ResourceMultiTexturepackerPtr;
    //////////////////////////////////////////////////////////////////////////
}