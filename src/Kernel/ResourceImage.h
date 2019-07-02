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
        DECLARE_VISITABLE( Resource );

    public:
        ResourceImage();
        ~ResourceImage() override;

    public:
        typedef Resource IntrusiveDerivedType;

    public:
        inline void setMaxSize( const mt::vec2f & _size );
        inline const mt::vec2f & getMaxSize() const;

        inline void setSize( const mt::vec2f & _size );
        inline const mt::vec2f & getSize() const;

        inline void setOffset( const mt::vec2f & _size );
        inline const mt::vec2f & getOffset() const;

        inline void setUVImage( const mt::uv4f & _uv );
        inline const mt::uv4f & getUVImage() const;

        inline void setUVAlpha( const mt::uv4f & _uv );
        inline const mt::uv4f & getUVAlpha() const;

        inline void setUVTextureImage( const mt::uv4f & _uv );
        inline const mt::uv4f & getUVTextureImage() const;
        
        inline void setUVTextureAlpha( const mt::uv4f & _uv );
        inline const mt::uv4f & getUVTextureAlpha() const;

        inline void setUVImageRotate( bool _rotate );
        inline bool isUVImageRotate() const;

        inline void setUVAlphaRotate( bool _rotate );
        inline bool isUVAlphaRotate() const;

        inline void setAlpha( bool _alpha );
        inline bool hasAlpha() const;

        inline void setPremultiply( bool _alpha );
        inline bool isPremultiply() const;

        inline void setPow2( bool _pow2 );
        inline bool isPow2() const;

    public:
        void setTexture( const RenderTextureInterfacePtr & _texture );
        inline const RenderTextureInterfacePtr & getTexture() const;

        void setTextureAlpha( const RenderTextureInterfacePtr & _textureAlpha );
        inline const RenderTextureInterfacePtr & getTextureAlpha() const;

        inline void setColor( const Color & _color );
        inline const Color & getColor() const;

    public:
        void correctUVImage( const mt::vec2f & _in, mt::vec2f * _out ) const;
        void correctUVAlpha( const mt::vec2f & _in, mt::vec2f * _out ) const;

    protected:
        void _release() override;

    protected:
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
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ResourceImage, Resource> ResourceImagePtr;
    //////////////////////////////////////////////////////////////////////////
    inline void ResourceImage::setMaxSize( const mt::vec2f & _maxSize )
    {
        m_maxSize = _maxSize;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const mt::vec2f & ResourceImage::getMaxSize() const
    {
        return m_maxSize;
    }
    //////////////////////////////////////////////////////////////////////////
    inline void ResourceImage::setSize( const mt::vec2f & _size )
    {
        m_size = _size;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const mt::vec2f & ResourceImage::getSize() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    inline void ResourceImage::setOffset( const mt::vec2f & _offset )
    {
        m_offset = _offset;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const mt::vec2f & ResourceImage::getOffset() const
    {
        return m_offset;
    }
    //////////////////////////////////////////////////////////////////////////
    inline void ResourceImage::setUVImage( const mt::uv4f & _uv )
    {
        m_uvImage = _uv;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const mt::uv4f & ResourceImage::getUVImage() const
    {
        return m_uvImage;
    }
    //////////////////////////////////////////////////////////////////////////
    inline void ResourceImage::setUVAlpha( const mt::uv4f & _uv )
    {
        m_uvAlpha = _uv;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const mt::uv4f & ResourceImage::getUVAlpha() const
    {
        return m_uvAlpha;
    }
    //////////////////////////////////////////////////////////////////////////
    inline void ResourceImage::setUVTextureImage( const mt::uv4f & _uv )
    {
        m_uvTextureImage = _uv;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const mt::uv4f & ResourceImage::getUVTextureImage() const
    {
        return m_uvTextureImage;
    }
    //////////////////////////////////////////////////////////////////////////
    inline void ResourceImage::setUVTextureAlpha( const mt::uv4f & _uv )
    {
        m_uvTextureAlpha = _uv;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const mt::uv4f & ResourceImage::getUVTextureAlpha() const
    {
        return m_uvTextureAlpha;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const RenderTextureInterfacePtr & ResourceImage::getTexture() const
    {
        return m_texture;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const RenderTextureInterfacePtr & ResourceImage::getTextureAlpha() const
    {
        return m_textureAlpha;
    }
    //////////////////////////////////////////////////////////////////////////
    inline void ResourceImage::setColor( const Color & _color )
    {
        m_color = _color;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const Color & ResourceImage::getColor() const
    {
        return m_color;
    }
    //////////////////////////////////////////////////////////////////////////
    inline void ResourceImage::setUVImageRotate( bool _rotate )
    {
        m_uvImageRotate = _rotate;
    }
    //////////////////////////////////////////////////////////////////////////
    inline bool ResourceImage::isUVImageRotate() const
    {
        return m_uvImageRotate;
    }
    //////////////////////////////////////////////////////////////////////////
    inline void ResourceImage::setUVAlphaRotate( bool _rotate )
    {
        m_uvAlphaRotate = _rotate;
    }
    //////////////////////////////////////////////////////////////////////////
    inline bool ResourceImage::isUVAlphaRotate() const
    {
        return m_uvAlphaRotate;
    }
    //////////////////////////////////////////////////////////////////////////
    inline void ResourceImage::setAlpha( bool _alpha )
    {
        m_hasAlpha = _alpha;
    }
    //////////////////////////////////////////////////////////////////////////
    inline bool ResourceImage::hasAlpha() const
    {
        return m_hasAlpha;
    }
    //////////////////////////////////////////////////////////////////////////
    inline void ResourceImage::setPremultiply( bool _premultiply )
    {
        m_isPremultiply = _premultiply;
    }
    //////////////////////////////////////////////////////////////////////////
    inline bool ResourceImage::isPremultiply() const
    {
        return m_isPremultiply;
    }
    //////////////////////////////////////////////////////////////////////////
    inline void ResourceImage::setPow2( bool _pow2 )
    {
        m_isPow2 = _pow2;
    }
    //////////////////////////////////////////////////////////////////////////
    inline bool ResourceImage::isPow2() const
    {
        return m_isPow2;
    }
}
