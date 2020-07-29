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
        void setResourceJSON( const ResourcePtr & _resourceJSON ) override;
        const ResourcePtr & getResourceJSON() const override;

    public:
        void setResourceImage( const ResourceImagePtr & _resourceImage ) override;
        const ResourceImagePtr & getResourceImage() const override;

    public:
        const ResourceImagePtr & getAtlasImage() const override;

    public:
        bool findFrame( const ConstString & _name, ResourceImagePtr * const _resourceImage ) const override;

    public:
        void visitFrames( const LambdaFrames & _lambdaFrames ) const override;

    public:
        void setStripFrameNameExtension( bool _value ) override;
        bool getStripFrameNameExtension() const override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        uint32_t m_atlasWidth;
        uint32_t m_atlasHeight;

        float m_atlasWidthInv;
        float m_atlasHeightInv;

        ResourcePtr m_resourceJSON;

        ResourceImagePtr m_resourceImage;
        bool m_extraResourceImage;

        bool m_needStripFrameNameExtension;

        typedef Hashtable<ConstString, ResourceImagePtr> HashtableTexturepackerFrames;
        HashtableTexturepackerFrames m_hashtableFrames;

        VectorResourceImages m_frames;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceTexturepacker> ResourceTexturepackerPtr;
    //////////////////////////////////////////////////////////////////////////
}