#	include "ResourceImageCNG.h"

#	include "FileEngine.h"
#	include "RenderEngine.h"

#	include "XmlParser.h"

#	include "../MngReader/MNG.h"

namespace Menge
{
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
	size_t ResourceImageCNG::getFrameCount()
	{
		return m_frames.size();
	}
	//////////////////////////////////////////////////////////////////////////
	int ResourceImageCNG::getFrameDelay( size_t _frame )
	{
		return m_frames[ _frame ].delay;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageCNG::getFrameSize( size_t _image )
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageCNG::getFrameOffset( size_t _image )
	{
		return m_image.offset;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageCNG::getFrameUV( size_t _frame )
	{
		return m_frames[ _frame ].uv;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface * ResourceImageCNG::getFrameImage( size_t _frame )
	{
		return m_image.renderImage;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageCNG::loader( TiXmlElement * _xml )
	{
		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_NODE("File")
			{
				XML_VALUE_ATTRIBUTE("Path", m_fileMNG );
			}		
		}

		ResourceImpl::loader( _xml );
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

		m_frames.resize(size);

		for( size_t index = 0; index < size; ++index )
		{
			fileData->read_ints(&m_frames[index].index,1);
			fileData->read_ints(&m_frames[index].delay,1);

			int offset = float( index ) / m_numY;

			m_frames[index].uv[0] = float( index % m_numX ) / m_numX;
			m_frames[index].uv[1] = float( offset ) / m_numY;
			m_frames[index].uv[2] = float( index % m_numX + 1 ) / m_numX;
			m_frames[index].uv[3] = float( offset + 1 ) / m_numY;
		}

		fileData->read_ints(&size,1);
		char* buffer = new char[size];
		fileData->read_chars(buffer,size);

		TextureDesc	textureDesc;
		textureDesc.buffer = buffer;
		textureDesc.size = size;
		textureDesc.haveAlpha = false;

		m_image.offset = mt::vec2f(0,0);
		m_image.renderImage = Holder<RenderEngine>::hostage()->loadImage(textureDesc);

		delete[] buffer;

		Holder<FileEngine>::hostage()->closeFile(fileData);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageCNG::_release()
	{
		Holder<RenderEngine>::hostage()
			->releaseRenderImage(m_image.renderImage);
	}
}