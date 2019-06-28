#pragma once

#include "Interface/RenderTextureInterface.h"
#include "Interface/RenderImageInterface.h"

#include "math/vec4.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class RenderTexture
        : public RenderTextureInterface
        , public Factorable
    {
    public:
        RenderTexture();
        ~RenderTexture() override;

    public:
        void initialize( uint32_t _id, const RenderImageInterfacePtr & _image, uint32_t _width, uint32_t _height );

    public:
        void release() override;

    public:
        const RenderImageInterfacePtr & getImage() const override;

    public:
        uint32_t getId() const override;

    public:
        void setCategory( const FileGroupInterfacePtr & _fileGroup ) override;
        const FileGroupInterfacePtr & getCategory() const override;

        void setFileName( const FilePath & _fileName ) override;
        const FilePath & getFileName() const override;

        const Rect & getRect() const override;

        const mt::uv4f & getUV() const override;

        uint32_t getWidth() const override;
        uint32_t getHeight() const override;

        float getWidthInv() const override;
        float getHeightInv() const override;

        bool isPow2() const override;

    protected:
        uint32_t m_id;

        RenderImageInterfacePtr m_image;

        FileGroupInterfacePtr m_fileGroup;
        FilePath m_fileName;

        uint32_t m_width;
        uint32_t m_height;

        float m_widthInv;
        float m_heightInv;

        Rect m_rect;
        mt::uv4f m_uv;

        bool m_pow2;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderTexture, RenderTextureInterface> RenderTexturePtr;
    //////////////////////////////////////////////////////////////////////////
    inline const FileGroupInterfacePtr & RenderTexture::getCategory() const
    {
        return m_fileGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const FilePath & RenderTexture::getFileName() const
    {
        return m_fileName;
    }
}
