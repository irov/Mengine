#pragma once

#   include "Interface/StreamInterface.h"
#   include "Interface/ImageCodecInterface.h"

#include "Kernel/ResourceReference.h"

#include "Core/ColourValue.h"

#include "math/vec4.h"
#include "math/uv4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<class RenderTextureInterface> RenderTextureInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class ResourceImage
		: public ResourceReference
	{
		DECLARE_VISITABLE( ResourceReference );

	public:
		ResourceImage();
		~ResourceImage();
			
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

        inline void setUVImageRotate( bool _rotate );
        inline bool isUVImageRotate() const;

        inline void setUVAlphaRotate( bool _rotate );
        inline bool isUVAlphaRotate() const;

		inline void setAlpha( bool _alpha );
		inline bool hasAlpha() const;

		inline void setPremultiply( bool _alpha );
		inline bool getPremultiply() const;
    
    public:
        void setTexture( const RenderTextureInterfacePtr & _texture );
		inline const RenderTextureInterfacePtr & getTexture() const;

        void setTextureAlpha( const RenderTextureInterfacePtr & _textureAlpha );
        inline const RenderTextureInterfacePtr & getTextureAlpha() const;

		inline void setColor( const ColourValue & _colour );
		inline const ColourValue & getColor() const;

    public:
        void correctUVImage( mt::vec2f & _out, const mt::vec2f & _in );
        void correctUVAlpha( mt::vec2f & _out, const mt::vec2f & _in );
        
    protected:
        void _release() override;

	protected:        
        RenderTextureInterfacePtr m_texture;
        RenderTextureInterfacePtr m_textureAlpha;

		ColourValue m_textureColor;

        mt::vec2f m_maxSize;
        mt::vec2f m_size;
		mt::vec2f m_offset;
        
		mt::uv4f m_uvImage;
        mt::uv4f m_uvAlpha;

        bool m_uvImageRotate;
        bool m_uvAlphaRotate;
        bool m_hasAlpha;
		bool m_isPremultiply;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ResourceImage> ResourceImagePtr;
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
	inline void ResourceImage::setColor( const ColourValue & _colour )
	{
		m_textureColor = _colour;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const ColourValue & ResourceImage::getColor() const
	{
		return m_textureColor;
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
	inline bool ResourceImage::getPremultiply() const
	{
		return m_isPremultiply;
	}
}
