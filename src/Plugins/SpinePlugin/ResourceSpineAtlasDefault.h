#pragma once

#include "ResourceSpineAtlas.h"

#include "Kernel/Resource.h"
#include "Kernel/ResourceImage.h"

#include "spine/spine.h"

namespace Mengine
{
    class ResourceSpineAtlasDefault
        : public ResourceSpineAtlas
    {
        DECLARE_VISITABLE( ResourceSpineAtlas );

    public:
        ResourceSpineAtlasDefault();
        ~ResourceSpineAtlasDefault() override;

    public:
        void addResourceImageDesc( const ConstString & _name, const ResourceImagePtr & _resourceImage );

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
