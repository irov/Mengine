#pragma once

#include "Interface/ImageCodecInterface.h"

#include "Kernel/Resource.h"

#include "Kernel/Color.h"

#include "math/vec4.h"
#include "math/uv4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class RenderTextureInterface> RenderTextureInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class ResourceImage
        : public Resource
    {
        DECLARE_FACTORABLE( ResourceImage );
        DECLARE_VISITABLE( Resource );

    public:
        ResourceImage();
        ~ResourceImage() override;

    public:
        MENGINE_INLINE void setMaxSize( const mt::vec2f & _size );
        MENGINE_INLINE const mt::vec2f & getMaxSize() const;

        MENGINE_INLINE void setSize( const mt::vec2f & _size );
        MENGINE_INLINE const mt::vec2f & getSize() const;

        MENGINE_INLINE void setOffset( const mt::vec2f & _size );
        MENGINE_INLINE const mt::vec2f & getOffset() const;

        MENGINE_INLINE void setUVImage( const mt::uv4f & _uv );
        MENGINE_INLINE const mt::uv4f & getUVImage() const;

        MENGINE_INLINE void setUVAlpha( const mt::uv4f & _uv );
        MENGINE_INLINE const mt::uv4f & getUVAlpha() const;

        MENGINE_INLINE void setUVTextureImage( const mt::uv4f & _uv );
        MENGINE_INLINE const mt::uv4f & getUVTextureImage() const;

        MENGINE_INLINE void setUVTextureAlpha( const mt::uv4f & _uv );
        MENGINE_INLINE const mt::uv4f & getUVTextureAlpha() const;

        MENGINE_INLINE void setUVImageRotate( bool _rotate );
        MENGINE_INLINE bool isUVImageRotate() const;

        MENGINE_INLINE void setUVAlphaRotate( bool _rotate );
        MENGINE_INLINE bool isUVAlphaRotate() const;

        MENGINE_INLINE void setAlpha( bool _alpha );
        MENGINE_INLINE bool hasAlpha() const;

        MENGINE_INLINE void setPremultiply( bool _premultiply );
        MENGINE_INLINE bool isPremultiply() const;

        MENGINE_INLINE void setPow2( bool _pow2 );
        MENGINE_INLINE bool isPow2() const;

    public:
        void setTexture( const RenderTextureInterfacePtr & _texture );
        MENGINE_INLINE const RenderTextureInterfacePtr & getTexture() const;

        void setTextureAlpha( const RenderTextureInterfacePtr & _textureAlpha );
        MENGINE_INLINE const RenderTextureInterfacePtr & getTextureAlpha() const;

        MENGINE_INLINE void setColor( const Color & _color );
        MENGINE_INLINE const Color & getColor() const;

    public:
        void correctUVImage( const mt::vec2f & _in, mt::vec2f * const _out ) const;
        void correctUVAlpha( const mt::vec2f & _in, mt::vec2f * const _out ) const;

    protected:
        bool _compile() override;
        void _release() override;

    private:
        RenderTextureInterfacePtr m_texture;
        RenderTextureInterfacePtr m_textureAlpha;

        Color m_color;

        mt::vec2f m_maxSize;
        mt::vec2f m_size;
        mt::vec2f m_offset;

        mt::uv4f m_uvImage;
        mt::uv4f m_uvAlpha;

        mt::uv4f m_uvTextureImage;
        mt::uv4f m_uvTextureAlpha;

        bool m_uvImageRotate;
        bool m_uvAlphaRotate;
        bool m_hasAlpha;
        bool m_isPremultiply;
        bool m_isPow2;
        bool m_isUVImageIdentity;
        bool m_isUVAlphaIdentity;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceImage> ResourceImagePtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE void ResourceImage::setMaxSize( const mt::vec2f & _maxSize )
    {
        m_maxSize = _maxSize;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const mt::vec2f & ResourceImage::getMaxSize() const
    {
        return m_maxSize;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE void ResourceImage::setSize( const mt::vec2f & _size )
    {
        m_size = _size;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const mt::vec2f & ResourceImage::getSize() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE void ResourceImage::setOffset( const mt::vec2f & _offset )
    {
        m_offset = _offset;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const mt::vec2f & ResourceImage::getOffset() const
    {
        return m_offset;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE void ResourceImage::setUVImage( const mt::uv4f & _uv )
    {
        m_uvImage = _uv;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const mt::uv4f & ResourceImage::getUVImage() const
    {
        return m_uvImage;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE void ResourceImage::setUVAlpha( const mt::uv4f & _uv )
    {
        m_uvAlpha = _uv;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const mt::uv4f & ResourceImage::getUVAlpha() const
    {
        return m_uvAlpha;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE void ResourceImage::setUVTextureImage( const mt::uv4f & _uv )
    {
        m_uvTextureImage = _uv;

        m_isUVImageIdentity = mt::uv4_is_identity( m_uvTextureImage );
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const mt::uv4f & ResourceImage::getUVTextureImage() const
    {
        return m_uvTextureImage;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE void ResourceImage::setUVTextureAlpha( const mt::uv4f & _uv )
    {
        m_uvTextureAlpha = _uv;

        m_isUVAlphaIdentity = mt::uv4_is_identity( m_uvTextureAlpha );
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const mt::uv4f & ResourceImage::getUVTextureAlpha() const
    {
        return m_uvTextureAlpha;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const RenderTextureInterfacePtr & ResourceImage::getTexture() const
    {
        return m_texture;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const RenderTextureInterfacePtr & ResourceImage::getTextureAlpha() const
    {
        return m_textureAlpha;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE void ResourceImage::setColor( const Color & _color )
    {
        m_color = _color;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const Color & ResourceImage::getColor() const
    {
        return m_color;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE void ResourceImage::setUVImageRotate( bool _rotate )
    {
        m_uvImageRotate = _rotate;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool ResourceImage::isUVImageRotate() const
    {
        return m_uvImageRotate;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE void ResourceImage::setUVAlphaRotate( bool _rotate )
    {
        m_uvAlphaRotate = _rotate;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool ResourceImage::isUVAlphaRotate() const
    {
        return m_uvAlphaRotate;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE void ResourceImage::setAlpha( bool _alpha )
    {
        m_hasAlpha = _alpha;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool ResourceImage::hasAlpha() const
    {
        return m_hasAlpha;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE void ResourceImage::setPremultiply( bool _premultiply )
    {
        m_isPremultiply = _premultiply;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool ResourceImage::isPremultiply() const
    {
        return m_isPremultiply;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE void ResourceImage::setPow2( bool _pow2 )
    {
        m_isPow2 = _pow2;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool ResourceImage::isPow2() const
    {
        return m_isPow2;
    }
    //////////////////////////////////////////////////////////////////////////
}
