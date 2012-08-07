#	include "ResourceImplement.h"
#	include "ResourceImageSolid.h"
#	include "LogEngine.h"
#	include "RenderEngine.h"
#	include "Metacode.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceImageSolid );
	//////////////////////////////////////////////////////////////////////////
	ResourceImageSolid::ResourceImageSolid()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageSolid::getMaxSize() const
	{
		return m_imageFrame.maxSize;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageSolid::getSize() const
	{
		return m_imageFrame.size;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageSolid::getOffset() const
	{
		return m_imageFrame.offset;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageSolid::getUV() const
	{
		return m_imageFrame.uv;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageSolid::getUVImage() const
	{
		return m_imageFrame.uv_image;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderTextureInterface* ResourceImageSolid::getTexture() const
	{
		return m_imageFrame.texture;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageSolid::isAlpha() const
	{
		return m_imageFrame.isAlpha;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageSolid::getWrapX() const 
	{
		return m_imageFrame.wrapX;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageSolid::getWrapY() const 
	{
		return m_imageFrame.wrapY;
	}
	//////////////////////////////////////////////////////////////////////////
	const WString & ResourceImageSolid::getFileName() const 
	{
		return m_fileName;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceImageSolid::getCodecType() const 
	{
		return m_codecType;
	}
	//////////////////////////////////////////////////////////////////////////
	const ColourValue & ResourceImageSolid::getColor() const 
	{
		return m_color;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageSolid::loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceImageSolid * metadata 
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceImageSolid *>(_meta);


        m_imageFrame.size = metadata->get_Size_Value();
        m_color = metadata->get_Color_Value();

        m_imageFrame.maxSize = m_imageFrame.size;
        m_imageFrame.uv = mt::vec4f(0.f,0.f,1.f,1.f);
        m_imageFrame.isAlpha = false;
        m_imageFrame.wrapX = true;
        m_imageFrame.wrapY = true;
        m_imageFrame.texture = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageSolid::_compile()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageSolid::_release()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageSolid::isValid() const 
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageSolid::loadBuffer( unsigned char * _buffer, int _pitch )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
}