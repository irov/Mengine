#	include "ResourceImageMNG.h"
#	include "ResourceImplement.h"

#	include "FileEngine.h"
#	include "RenderEngine.h"

#	include "XmlParser.h"

namespace Menge
{
	RESOURCE_IMPLEMENT( ResourceImageMNG )
	//////////////////////////////////////////////////////////////////////////
	ResourceImageMNG::ResourceImageMNG( const std::string & _name )
		: ResourceImage( _name )
		, m_uv( 0.f, 0.f, 1.f, 1.f )
	{}
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

		int size;

		fileData->read_ints(&size,1);

		int x, y;
		fileData->read_ints(&x,1);
		fileData->read_ints(&y,1);

		m_size = mt::vec2f(x,y);
	
		TextureDesc	textureDesc;

		m_images.reserve(size);

		for(int i = 0; i < size; ++i)
		{
			fileData->read_ints(&x,1);
			fileData->read_ints(&y,1);

			int bsize = 0;
			fileData->read_ints(&bsize,1);

			char* buffer = new char[bsize];
			fileData->read_chars(buffer,bsize);

			textureDesc.buffer = buffer;
			textureDesc.size = bsize;
			textureDesc.haveAlpha = true;

			Image	imageProps;

			imageProps.offset = mt::vec2f(x,y);
			imageProps.renderImage = Holder<RenderEngine>::hostage()->loadImage(textureDesc);
			imageProps.size = mt::vec2f(imageProps.renderImage->getWidth(),imageProps.renderImage->getHeight());

			m_images.push_back(imageProps);

			delete[] buffer;
		}

		Holder<FileEngine>::hostage()->closeFile(fileData);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageMNG::_release()
	{
		for each( const Image & img in m_images )
		{
			Holder<RenderEngine>::hostage()
				->releaseRenderImage( img.renderImage );
		}
	}
}