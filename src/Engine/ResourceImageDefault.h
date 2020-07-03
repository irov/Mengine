#pragma once

#include "Kernel/ResourceImage.h"

#include "Kernel/BaseContent.h"

namespace Mengine
{
    class ResourceImageDefault
        : public ResourceImage
        , private BaseContent
    {
        DECLARE_VISITABLE( ResourceImage );
        DECLARE_CONTENTABLE();

    public:
        ResourceImageDefault();
        ~ResourceImageDefault() override;

    public:
        bool setup( const FileGroupInterfacePtr & _fileGroup, const FilePath & _imagePath, const ConstString & _codecType, const mt::uv4f & _uv_image, const mt::uv4f & _uv_alpha, const mt::vec2f & _maxSize );

    protected:
        bool _compile() override;

    public:
        void correctUVTexture();

    protected:
        void prepareImageFrame_();        
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceImageDefault> ResourceImageDefaultPtr;
    //////////////////////////////////////////////////////////////////////////
}