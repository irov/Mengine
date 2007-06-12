#	include "ResourceImageMNG.h"
#	include "ResourceImplement.h"

#	include "FileEngine.h"
#	include "RenderEngine.h"

#	include "XmlParser.h"

#	include "../MngReader/MNG.h"

namespace Menge
{
	RESOURCE_IMPLEMENT( ResourceImageMNG )
	//////////////////////////////////////////////////////////////////////////
	ResourceImageMNG::ResourceImageMNG( const std::string & _name )
		: ResourceImage( _name )
		, m_uv( 0.f, 0.f, 1.f, 1.f )
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageMNG::getMaxSize()
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceImageMNG::getCount()
	{
		return m_images.size();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageMNG::getUV( size_t _index )
	{
		return m_uv;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageMNG::getSize( size_t _index )
	{
		return m_images[ _index ].size;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageMNG::getOffset( size_t _index )
	{
		return m_images[ _index ].offset;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface * ResourceImageMNG::getImage( size_t _index )
	{
		return m_images[ _index ].renderImage;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageMNG::loader( TiXmlElement * _xml )
	{
		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_VALUE_NODE( "File", "Path", m_fileMNG );
		}

		ResourceImpl::loader( _xml );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageMNG::_compile()
	{
		FileDataInterface* fileData = Holder<FileEngine>::hostage()->openFile(m_fileMNG);

		assert(fileData != 0);

		mnglib::mngDesc	desc;

		readMNG(
			desc,
			(unsigned char*)fileData->getBuffer(),
			fileData->size()
			);

		Holder<FileEngine>::hostage()->closeFile(fileData);

		m_size.x = desc.width;
		m_size.y = desc.height;

		TextureDesc	textureDesc;

		size_t size = desc.images.size();

		for(size_t i = 0; i < size; i++)
		{
			textureDesc.buffer = desc.images[i].buffer;
			textureDesc.size = desc.images[i].size;
			textureDesc.haveAlpha = true;

			Image	imageProps;

			imageProps.offset = mt::vec2f(
				(float)desc.images[i].offsetX, 
				(float)desc.images[i].offsetY);

			imageProps.renderImage = Holder<RenderEngine>::hostage()->loadImage(textureDesc);
			imageProps.size = mt::vec2f(imageProps.renderImage->getWidth(),imageProps.renderImage->getHeight());
			m_images.push_back(imageProps);
		}
		freeMNG(desc);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageMNG::_release()
	{
		for( size_t i = 0, size = m_images.size(); i < size; ++i )
		{
			Holder<RenderEngine>::hostage()
				->releaseRenderImage(m_images[i].renderImage);
		}
	}
}