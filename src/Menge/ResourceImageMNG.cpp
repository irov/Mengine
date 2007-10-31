#	include "ResourceImageMNG.h"
#	include "ResourceImplement.h"

#	include "FileEngine.h"
#	include "RenderEngine.h"

#	include "XmlParser/XmlParser.h"

#	include <sstream>

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
	size_t ResourceImageMNG::getCount()
	{
		return m_images.size();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageMNG::getMaxSize( size_t _frame ) const
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageMNG::getUV( size_t _frame ) const
	{
		return m_uv;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageMNG::getSize( size_t _frame ) const
	{
		return m_images[ _frame ].size;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageMNG::getOffset( size_t _frame ) const
	{
		return m_images[ _frame ].offset;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderImageInterface * ResourceImageMNG::getImage( size_t _frame ) const
	{
		return m_images[ _frame ].renderImage;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageMNG::loader( TiXmlElement * _xml )
	{
		ResourceImage::loader( _xml );

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_VALUE_NODE( "File", "Path", m_fileMNG );
			XML_CHECK_VALUE_NODE( "Filter", "Value", m_filter );
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageMNG::_compile()
	{
		FileDataInterface* fileData = Holder<FileEngine>::hostage()->openFile(m_fileMNG);

		assert(fileData != 0);

		int x, y;
		fileData->read(&x,sizeof(int));
		fileData->read(&y,sizeof(int));
		m_size = mt::vec2f( float(x), float(y) );

		int size;
		fileData->read(&size,sizeof(int));

		std::vector<long> seeks(size);

		for(size_t i = 0; i < seeks.size(); ++i)
		{
			fileData->read(&seeks[i],sizeof(long));
		}

		TextureDesc	textureDesc;

		m_images.reserve(size);

		std::vector<char> buff;
		for(int i = 0; i < size; ++i)
		{
			fileData->read(&x,sizeof(int));
			fileData->read(&y,sizeof(int));

			int bsize = 0;
			fileData->read(&bsize,sizeof(int));

			buff.resize( bsize );
			fileData->read( &buff[0], bsize );

			textureDesc.buffer = &buff[0];
			textureDesc.size = bsize;
			textureDesc.filter = m_filter;

			std::stringstream str;
			str << m_fileMNG << i;
			std::string mngimagename = str.str();


			size_t index = mngimagename.find_last_of(".");
			std::string ext = mngimagename.substr( 0, index + 1 );
			ext += "png";

			textureDesc.name = ext.c_str();

			Image	imageProps;

			imageProps.offset = mt::vec2f( float(x), float(y) );
			
			imageProps.renderImage = 
				Holder<RenderEngine>::hostage()
				->loadImage(textureDesc);

			float width = (float)imageProps.renderImage->getWidth();
			float height = (float)imageProps.renderImage->getHeight();

			imageProps.size = mt::vec2f( width, height );

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
				->releaseImage( img.renderImage );
		}
	}
}