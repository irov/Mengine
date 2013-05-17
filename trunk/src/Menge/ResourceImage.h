#	pragma once

#   include "Interface/StreamInterface.h"
#   include "Interface/RenderSystemInterface.h"
#   include "Interface/ImageCodecInterface.h"

#	include "Kernel/ResourceReference.h"

#	include "Math/vec4.h"

namespace Menge
{
	class ResourceImage
		: public ResourceReference
	{
	public:
		ResourceImage();

	public:        
        inline const mt::vec2f & getTextureSize() const;
		inline const mt::vec2f & getMaxSize() const;
		inline const mt::vec2f & getSize() const;
        inline const mt::vec2f & getBound() const;
		inline const mt::vec4f & getUV() const;
        inline const mt::vec4f & getUVScale() const;
		inline const mt::vec4f & getUVImage() const;
        inline const mt::vec4f & getUVAlpha() const;

		inline bool isAlpha() const;
        inline bool isWrapX() const;
        inline bool isWrapY() const;
        inline bool isUVRotate() const;
    
    public:
		inline const RenderTextureInterfacePtr & getTexture() const;
        inline const RenderTextureInterfacePtr & getTextureAlpha() const;
        
	protected:
		bool loadImageFrame_( const ConstString& _pakName, const FilePath& _filename, const ConstString& _codec );
		
		//bool createImageFrame_( ImageFrame & _frame, const ConstString& _name, const mt::vec2f& _size ) const;
		bool prepareImageFrame_( const RenderTextureInterfacePtr & _texture );

	protected:
		ImageDecoderInterfacePtr createDecoder_( const InputStreamInterfacePtr & _stream,  const ConstString& _codec ) const;

    protected:
        void _release() override;
		
	protected:        
        RenderTextureInterfacePtr m_texture;
        RenderTextureInterfacePtr m_textureAlpha;

        mt::vec2f m_textureSize;
        mt::vec2f m_maxSize;
        mt::vec2f m_size;

        mt::vec4f m_uv_image;
        mt::vec4f m_uv;
        mt::vec4f m_uv_scale;

        mt::vec4f m_uv_alpha;

        bool m_isAlpha;
        bool m_isUVRotate;

        bool m_wrapX;
        bool m_wrapY;        
	};
    //////////////////////////////////////////////////////////////////////////
    inline const mt::vec2f & ResourceImage::getTextureSize() const
    {
        return m_textureSize;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const mt::vec2f & ResourceImage::getMaxSize() const
    {
        return m_maxSize;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const mt::vec2f & ResourceImage::getSize() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const mt::vec4f & ResourceImage::getUV() const
    {
        return m_uv;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const mt::vec4f & ResourceImage::getUVScale() const
    {
        return m_uv_scale;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const mt::vec4f & ResourceImage::getUVImage() const
    {
        return m_uv_image;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const mt::vec4f & ResourceImage::getUVAlpha() const
    {
        return m_uv_alpha;
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
    inline bool ResourceImage::isAlpha() const
    {
        return m_isAlpha;
    }
    inline bool ResourceImage::isUVRotate() const
    {
        return m_isUVRotate;
    }
    //////////////////////////////////////////////////////////////////////////
    inline bool ResourceImage::isWrapX() const
    {
        return m_wrapX;
    }
    //////////////////////////////////////////////////////////////////////////
    inline bool ResourceImage::isWrapY() const
    {
        return m_wrapY;
    }
}
