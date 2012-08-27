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
	bool ResourceImageSolid::loadBufferAlpha( unsigned char * _buffer, int _pitch )
	{        
        size_t width = m_imageFrame.size.x;
        size_t height = m_imageFrame.size.y;

        if( _pitch < width )
        {
            return false;
        }

        for( size_t j = 0; j != height; ++j )        
        {
            for( size_t i = 0; i != width; ++i )    
            {
                _buffer[i] = 255;
            }

            _buffer += _pitch;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}