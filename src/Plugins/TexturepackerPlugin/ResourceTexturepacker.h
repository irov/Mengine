#pragma once

#include "TexturepackerInterface.h"

#include "Kernel/Resource.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/Hashtable.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceTexturepacker
        : public Resource
        , public UnknownResourceTexturepackerInterface
    {
        DECLARE_VISITABLE( Resource );
        DECLARE_UNKNOWABLE();

    public:
        ResourceTexturepacker();
        ~ResourceTexturepacker() override;

    public:
        uint32_t getAtlasWidth() const override;
        uint32_t getAtlasHeight() const override;

        float getAtlasWidthInv() const override;
        float getAtlasHeightInv() const override;

    public:
        void setResourceJSONName( const ConstString & _resourceJSONName ) override;
        const ConstString & getResourceJSONName() const override;

    public:
        void setResourceImageName( const ConstString & _resourceImageName ) override;
        const ConstString & getResourceImageName() const override;

    public:
        const ResourceImagePtr & getAtlasImage() const override;

    public:
        const ResourceImagePtr & getFrame( const ConstString & _name ) const override;

    public:
        const VectorResourceImages & getFrames() const override;

    public:
        void setStripFrameNameExtension( bool _value ) override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        uint32_t m_atlasWidth;
        uint32_t m_atlasHeight;

        float m_atlasWidthInv;
        float m_atlasHeightInv;

        ConstString m_resourceJSONName;
        ResourcePtr m_resourceJSON;

        ConstString m_resourceImageName;
        ResourceImagePtr m_resourceImage;

        bool m_needStripFrameNameExtension;

        typedef Hashtable<ConstString, ResourceImagePtr> HashtableTexturepackerFrames;
        HashtableTexturepackerFrames m_hashtableFrames;

        VectorResourceImages m_frames;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceTexturepacker> ResourceTexturepackerPtr;
    //////////////////////////////////////////////////////////////////////////
}