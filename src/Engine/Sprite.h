#pragma once

#include "ShapeQuadFixed.h"

#include "SpriteCache.h"
#include "SurfaceImage.h"

#include "Kernel/ResourceImage.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Sprite
        : public ShapeQuadFixed
    {
        DECLARE_VISITABLE( Node );

    public:
        Sprite();
        ~Sprite() override;

    public:
        void setSpriteCache( const SpriteCachePtr & _spriteCache );
        const SpriteCachePtr & getSpriteCache() const;

    public:
        void setResourceImage( const ResourceImagePtr & _resourceImage );
        const ResourceImagePtr & getResourceImage() const;

    public:
        void setFileGroup( const FileGroupInterfacePtr & _fileGroup );
        const FileGroupInterfacePtr & getFileGroup() const;

        void setFilePath( const FilePath & _filePath );
        const FilePath & getFilePath() const;

        void setCodecType( const ConstString & _codecType );
        const ConstString & getCodecType() const;

        void setSize( const mt::vec2f & _size );
        const mt::vec2f & getSize() const;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        SurfacePtr makeSurfaceImage_();

    protected:
        SpriteCachePtr m_spriteCache;
        ResourceImagePtr m_resourceImage;

        FileGroupInterfacePtr m_fileGroup;
        FilePath m_filePath;

        ConstString m_codecType;
        mt::vec2f m_size;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Sprite> SpritePtr;
    //////////////////////////////////////////////////////////////////////////
}