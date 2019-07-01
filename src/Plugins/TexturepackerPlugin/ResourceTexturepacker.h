#pragma once

#include "TexturepackerInterface.h"

#include "Kernel/Resource.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/Hashtable.h"

#include "Config/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceTexturepacker
        : public Resource
        , public UnknownResourceTexturepackerInterface
    {
        DECLARE_VISITABLE(Resource);
        DECLARE_UNKNOWABLE();

    public:
        ResourceTexturepacker();
        ~ResourceTexturepacker() override;

    public:
        void setResourceJSONName( const ConstString& _resourceJSONName );
        const ConstString& getResourceJSONName() const;

    public:
        void setResourceImageName(const ConstString& _resourceImageName);
        const ConstString& getResourceImageName() const;

    public:
        const ResourceImagePtr & getFrame( const ConstString & _name ) const override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        ConstString m_resourceJSONName;
        ResourcePtr m_resourceJSON;

        ConstString m_resourceImageName;
        ResourceImagePtr m_resourceImage;

        typedef Hashtable<ConstString, ResourceImagePtr> HashtableTexturepackerFrames;
        HashtableTexturepackerFrames m_frames;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceTexturepacker> ResourceImageTexturepackerPtr;
    //////////////////////////////////////////////////////////////////////////
}