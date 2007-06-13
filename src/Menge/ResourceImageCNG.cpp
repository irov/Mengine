#	include "ResourceImageCNG.h"
#	include "ResourceImplement.h"

#	include "FileEngine.h"
#	include "RenderEngine.h"

#	include "XmlParser.h"

namespace Menge
{
	RESOURCE_IMPLEMENT( ResourceImageCNG )
	//////////////////////////////////////////////////////////////////////////
	ResourceImageCNG::ResourceImageCNG( const std::string & _name )
		: ResourceImage( _name )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageCNG::getMaxSize()
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceImageCNG::getCount()
	{
		return m_uvs.size();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageCNG::getSize( size_t _image )
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageCNG::getOffset( size_t _image )
	{
		return m_offset;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageCNG::getUV( size_t _index )
	{
		return m_uvs[ _index ];
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface * ResourceImageCNG::getImage( size_t _index )
	{
		return m_renderImage;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageCNG::loader( TiXmlElement * _xml )
	{
		ResourceImpl::loader( _xml );

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_VALUE_NODE( "File", "Path", m_fileMNG );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageCNG::_compile()
	{
		FileDataInterface* fileData = Holder<FileEngine>::hostage()->openFile(m_fileMNG);

		assert(fileData != 0);

		int cellWidth;
		int cellHeight;

		fileData->read_ints(&cellWidth,1);
		fileData->read_ints(&cellHeight,1);

		m_size.x = cellWidth;
		m_size.y = cellHeight;

		fileData->read_ints(&m_numX,1);
		fileData->read_ints(&m_numY,1);

		int size = m_numX*m_numY;

		m_uvs.resize(size);

		for( size_t index = 0; index < size; ++index )
		{
			int offset = float( index ) / m_numY;

			m_uvs[index].x = float( index % m_numX ) / m_numX;
			m_uvs[index].y = float( offset ) / m_numY;
			m_uvs[index].z = float( index % m_numX + 1 ) / m_numX;
			m_uvs[index].w = float( offset + 1 ) / m_numY;
		}

		fileData->read_ints(&size,1);
		char* buffer = new char[size];
		fileData->read_chars(buffer,size);

		TextureDesc	textureDesc;
		textureDesc.buffer = buffer;
		textureDesc.size = size;
		textureDesc.haveAlpha = false;

		m_offset = mt::vec2f( 0.f, 0.f );
		m_renderImage = Holder<RenderEngine>::hostage()->loadImage(textureDesc);

		delete[] buffer;

		Holder<FileEngine>::hostage()->closeFile(fileData);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageCNG::_release()
	{
		Holder<RenderEngine>::hostage()
			->releaseRenderImage( m_renderImage );
	}
}