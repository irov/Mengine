#pragma once

#include "ResourceSpineAtlas.h"

#include "Kernel/Resource.h"
#include "Kernel/Content.h"
#include "Kernel/ResourceImage.h"

#include "spine/spine.h"

namespace Mengine
{
    class ResourceSpineAtlasDefault
        : public ResourceSpineAtlas
        , public Content
    {
        DECLARE_VISITABLE( ResourceSpineAtlas );
        DECLARE_CONTENTABLE();

    public:
        ResourceSpineAtlasDefault();
        ~ResourceSpineAtlasDefault() override;

    public:
        void addResourceImageDesc( const ConstString & _name, const ConstString & _resourceImageName );

    protected:
        const ResourceImagePtr & getResourceImage_( const Char * _name ) const;

    public:
        spAtlas * getSpineAtlas() const override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        struct ImageDesc
        {
            ConstString name;
            ConstString resourceImageName;
            ResourceImagePtr resourceImage;
        };

        typedef Vector<ImageDesc> VectorImageDesc;
        VectorImageDesc m_images;

        spAtlas * m_atlas;

    protected:
        friend const ResourceImagePtr & _spAtlasPage_createTexture2( const ResourceSpineAtlasDefault *, const char * );
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceSpineAtlasDefault> ResourceSpineAtlasDefaultPtr;
    //////////////////////////////////////////////////////////////////////////
}
