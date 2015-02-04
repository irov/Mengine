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
		~ResourceImage();
			
	public:        
		inline const mt::vec2f & getMaxSize() const;
		inline const mt::vec2f & getSize() const;
		inline const mt::vec2f & getOffset() const;
		inline const mt::vec4f & getUV() const;
        inline const mt::vec4f & getUVScale() const;
		inline const mt::vec4f & getUVImage() const;
        inline const mt::vec4f & getUVAlpha() const;

		inline bool isAlpha() const;
        inline bool isWrapU() const;
        inline bool isWrapV() const;
        inline bool isUVRotate() const;
    
    public:
		inline const RenderTextureInterfacePtr & getTexture() const;
        inline const RenderTextureInterfacePtr & getTextureAlpha() const;
        
	protected:
		bool loadImageFrame_( const ConstString& _pakName, const FilePath& _fileName, const ConstString& _codecType );
		void prepareImageFrame_( const RenderTextureInterfacePtr & _texture );


    protected:
        void _release() override;

	protected:
		size_t memoryUse() const override;
		
	protected:        
        RenderTextureInterfacePtr m_texture;
        RenderTextureInterfacePtr m_textureAlpha;

        mt::vec2f m_maxSize;
        mt::vec2f m_size;
		mt::vec2f m_offset;

        mt::vec4f m_uv_image;
        mt::vec4f m_uv;
        mt::vec4f m_uv_scale;

        mt::vec4f m_uv_alpha;

        bool m_isAlpha;
        bool m_isUVRotate;

        bool m_wrapU;
        bool m_wrapV;        
	};
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
	inline const mt::vec2f & ResourceImage::getOffset() const
	{
		return m_offset;
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
    inline bool ResourceImage::isWrapU() const
    {
        return m_wrapU;
    }
    //////////////////////////////////////////////////////////////////////////
    inline bool ResourceImage::isWrapV() const
    {
        return m_wrapV;
    }
}
