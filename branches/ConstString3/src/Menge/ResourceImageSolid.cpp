#	include "ResourceImplement.h"
#	include "ResourceImageSolid.h"
#	include "LogEngine.h"
#	include "RenderEngine.h"
#	include "BinParser.h"

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
	void ResourceImageSolid::loader( BinParser * _parser )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE( Protocol::Size_Value, m_imageFrame.size );
			BIN_CASE_ATTRIBUTE( Protocol::Color_Value, m_color );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageSolid::_compile()
	{
		m_imageFrame.uv = mt::vec4f(0.f,0.f,1.f,1.f);
		m_imageFrame.offset = mt::vec2f(0.f,0.f);
		m_imageFrame.maxSize = m_imageFrame.size;
		m_imageFrame.isAlpha = false; //
		m_imageFrame.wrapX = true;
		m_imageFrame.wrapY = true;
		m_imageFrame.texture = NULL;

		/*const mt::vec2f& size = m_imageFrame.size;
		m_imageFrame.texture = RenderEngine::get()
			->createTexture( size.x, size.y, PF_X8R8G8B8 );

		if( m_imageFrame.texture == NULL )
		{
			MENGE_LOG_ERROR("Texture::ResourceImageSolid: Invalid create texture");
			return false;
		}*/

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageSolid::_release()
	{
		/*if( m_imageFrame.texture == NULL )
		{
			return;
		}

		RenderEngine::get()
			->releaseTexture(m_imageFrame.texture);

		m_imageFrame.texture = NULL;*/
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageSolid::isValid() const 
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}