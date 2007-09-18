#	include "ResourceImageMNG.h"
#	include "ResourceImplement.h"

#	include "FileEngine.h"
#	include "RenderEngine.h"

#	include "XmlParser/XmlParser.h"

namespace Menge
{
	RESOURCE_IMPLEMENT( ResourceImageMNG )
	//////////////////////////////////////////////////////////////////////////
	ResourceImageMNG::ResourceImageMNG( const std::string & _name )
		: ResourceImage( _name )
		, m_uv( 0.f, 0.f, 1.f, 1.f )
		, m_filter(1)
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
			XML_CHECK_VALUE_NODE( "Filter", "Value", m_filter );
		}

		ResourceImpl::loader( _xml );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageMNG::_compile()
	{
		FileDataInterface* fileData = Holder<FileEngine>::hostage()->openFile(m_fileMNG);

		assert(fileData != 0);

		int x, y;
		fileData->read_ints(&x,1);
		fileData->read_ints(&y,1);
		m_size = mt::vec2f( float(x), float(y) );

		int size;
		fileData->read_ints(&size,1);

		std::vector<long> seeks(size);

		for(size_t i = 0; i < seeks.size(); ++i)
		{
			fileData->read_longs(&seeks[i],1);
		}

		TextureDesc	textureDesc;

		m_images.reserve(size);

		for(int i = 0; i < size; ++i)
		{
			fileData->setPos( seeks[i] );

			fileData->read_ints(&x,1);
			fileData->read_ints(&y,1);

			int bsize = 0;
			fileData->read_ints(&bsize,1);

			textureDesc.buffer = (void*)fileData->getPos();
			textureDesc.size = bsize;
			textureDesc.filter = m_filter;

			Image	imageProps;

			imageProps.offset = mt::vec2f( float(x), float(y) );
			imageProps.renderImage = Holder<RenderEngine>::hostage()->loadImage(textureDesc);
			imageProps.size = mt::vec2f((float)imageProps.renderImage->getWidth(),(float)imageProps.renderImage->getHeight());

			m_images.push_back(imageProps);
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
	//////////////////////////////////////////////////////////////////////////
}