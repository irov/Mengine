#pragma once

#include "Interface/ImageCodecInterface.h"

#include "Kernel/Resource.h"

#include "Kernel/Color.h"

#include "math/vec4.h"
#include "math/uv4.h"

#ifndef MENGINE_RESOURCEIMAGE_MAX_TEXTURE
#define MENGINE_RESOURCEIMAGE_MAX_TEXTURE 2
#endif

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

        MENGINE_INLINE void setUV( size_t _index, const mt::uv4f & _uv );
        MENGINE_INLINE const mt::uv4f & getUV( size_t _index ) const;

        MENGINE_INLINE void setUVTexture( size_t _index, const mt::uv4f & _uv );
        MENGINE_INLINE const mt::uv4f & getUVTexture( size_t _index ) const;

        MENGINE_INLINE bool isUVIdentity( size_t _index ) const;

        MENGINE_INLINE void setUVRotate( size_t _index, bool _rotate );
        MENGINE_INLINE bool isUVRotate( size_t _index ) const;

        MENGINE_INLINE void setAlpha( bool _alpha );
        MENGINE_INLINE bool hasAlpha() const;

        MENGINE_INLINE void setPremultiply( bool _premultiply );
        MENGINE_INLINE bool isPremultiply() const;

        MENGINE_INLINE void setTrimAtlas( bool _trimAtlas );
        MENGINE_INLINE bool isTrimAtlas() const;

        MENGINE_INLINE void setPow2( bool _pow2 );
        MENGINE_INLINE bool isPow2() const;

    public:
        void setTexture( size_t _index, const RenderTextureInterfacePtr & _texture );
        MENGINE_INLINE const RenderTextureInterfacePtr & getTexture( size_t _index ) const;

        MENGINE_INLINE void setColor( const Color & _color );
        MENGINE_INLINE const Color & getColor() const;

    public:
        void correctUV( size_t _index, const mt::vec2f & _in, mt::vec2f * const _out ) const;

    protected:
        bool _compile() override;
        void _release() override;

    private:
        RenderTextureInterfacePtr m_texture[MENGINE_RESOURCEIMAGE_MAX_TEXTURE];

        Color m_color;

        mt::vec2f m_maxSize;
        mt::vec2f m_size;
        mt::vec2f m_offset;

        mt::uv4f m_uv[MENGINE_RESOURCEIMAGE_MAX_TEXTURE];
        mt::uv4f m_uvTexture[MENGINE_RESOURCEIMAGE_MAX_TEXTURE];
        bool m_uvRotate[MENGINE_RESOURCEIMAGE_MAX_TEXTURE];
        bool m_uvIdentity[MENGINE_RESOURCEIMAGE_MAX_TEXTURE];

        bool m_hasAlpha;
        bool m_isPremultiply;
        bool m_trimAtlas;
        bool m_isPow2;
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
    MENGINE_INLINE void ResourceImage::setUV( size_t _index, const mt::uv4f & _uv )
    {
        MENGINE_ASSERTION_FATAL( _index < MENGINE_RESOURCEIMAGE_MAX_TEXTURE, "resource image '%s' index %zu out of range"
            , this->getName().c_str()
            , _index
        );

        m_uv[_index] = _uv;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const mt::uv4f & ResourceImage::getUV( size_t _index ) const
    {
        MENGINE_ASSERTION_FATAL( _index < MENGINE_RESOURCEIMAGE_MAX_TEXTURE, "resource image '%s' index %zu out of range"
            , this->getName().c_str()
            , _index
        );

        return m_uv[_index];
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE void ResourceImage::setUVTexture( size_t _index, const mt::uv4f & _uv )
    {
        MENGINE_ASSERTION_FATAL( _index < MENGINE_RESOURCEIMAGE_MAX_TEXTURE, "resource image '%s' index %zu out of range"
            , this->getName().c_str()
            , _index
        );

        m_uvTexture[_index] = _uv;

        m_uvIdentity[_index] = mt::uv4_is_identity( _uv );
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const mt::uv4f & ResourceImage::getUVTexture( size_t _index ) const
    {
        MENGINE_ASSERTION_FATAL( _index < MENGINE_RESOURCEIMAGE_MAX_TEXTURE, "resource image '%s' index %zu out of range"
            , this->getName().c_str()
            , _index 
        );

        return m_uvTexture[_index];
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const RenderTextureInterfacePtr & ResourceImage::getTexture( size_t _index ) const
    {
        MENGINE_ASSERTION_FATAL( _index < MENGINE_RESOURCEIMAGE_MAX_TEXTURE, "resource image '%s' index %zu out of range"
            , this->getName().c_str()
            , _index
        );

        return m_texture[_index];
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
    MENGINE_INLINE bool ResourceImage::isUVIdentity( size_t _index ) const
    {
        MENGINE_ASSERTION_FATAL( _index < MENGINE_RESOURCEIMAGE_MAX_TEXTURE, "resource image '%s' index %zu out of range"
            , this->getName().c_str()
            , _index
        );

        return m_uvIdentity[_index];
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE void ResourceImage::setUVRotate( size_t _index, bool _rotate )
    {
        MENGINE_ASSERTION_FATAL( _index < MENGINE_RESOURCEIMAGE_MAX_TEXTURE, "resource image '%s' index %zu out of range"
            , this->getName().c_str()
            , _index
        );

        m_uvRotate[_index] = _rotate;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool ResourceImage::isUVRotate( size_t _index ) const
    {
        MENGINE_ASSERTION_FATAL( _index < MENGINE_RESOURCEIMAGE_MAX_TEXTURE, "resource image '%s' index %zu out of range"
            , this->getName().c_str()
            , _index
        );

        return m_uvRotate[_index];
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
    MENGINE_INLINE void ResourceImage::setTrimAtlas( bool _trimAtlas )
    {
        m_trimAtlas = _trimAtlas;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool ResourceImage::isTrimAtlas() const
    {
        return m_trimAtlas;
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
