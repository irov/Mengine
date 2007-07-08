#	include "ResourceImageDefault.h"

#	include "ResourceImplement.h"

#	include "FileEngine.h"
#	include "RenderEngine.h"

#	include "XmlParser.h"

namespace Menge
{
	RESOURCE_IMPLEMENT( ResourceImageDefault )
	//////////////////////////////////////////////////////////////////////////
	ResourceImageDefault::ResourceImageDefault( const std::string & _name )
		: ResourceImage( _name )
		, m_renderImage(0)
		, m_size(0.f, 0.f)
		, m_uv( 0.f, 0.f, 1.f, 1.f )
		, m_offset( 0.f, 0.f )
		, m_count(0)
		, m_filter(1)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageDefault::getMaxSize()
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceImageDefault::getCount()
	{
		return m_count;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageDefault::getSize( size_t _frame )
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageDefault::getOffset( size_t _image )
	{
		return m_offset;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageDefault::getUV( size_t _frame )
	{
		return m_uv;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface * ResourceImageDefault::getImage( size_t _image )
	{
		return m_renderImage;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDefault::loader( TiXmlElement * _xml )
	{
		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_VALUE_NODE( "File", "Path", m_fileImage );
			XML_CHECK_VALUE_NODE( "Filter", "Value", m_filter );
		}

		ResourceImpl::loader( _xml );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDefault::_compile()
	{
		FileDataInterface* fileData = Holder<FileEngine>::hostage()->openFile(m_fileImage);

		if( fileData == 0 )
		{
			return false;
		}

		TextureDesc	textureDesc;
		textureDesc.buffer = (void*)fileData->getBuffer();
		textureDesc.size = fileData->size();
		textureDesc.filter = m_filter;

		m_renderImage = Holder<RenderEngine>::hostage()->loadImage(textureDesc);

		if( m_renderImage == 0 )
		{
			return false;
		}

		m_size.x = m_renderImage->getWidth();
		m_size.y = m_renderImage->getHeight();

		m_count = 1;

		Holder<FileEngine>::hostage()->closeFile(fileData);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////()
	void ResourceImageDefault::_release()
	{
		Holder<RenderEngine>::hostage()
			->releaseRenderImage( m_renderImage );
	}
}