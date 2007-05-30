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
	{
		m_uv = mt::vec4f( 0.f, 0.f, 1.f, 1.f );
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageMNG::getMaxSize()
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceImageMNG::getFrameCount()
	{
		return m_frames.size();
	}
	//////////////////////////////////////////////////////////////////////////
	int ResourceImageMNG::getFrameDelay( size_t _frame )
	{
		return m_frames[ _frame ].delay;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageMNG::getFrameUV( size_t _frame )
	{
		return m_uv;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageMNG::getFrameSize( size_t _frame )
	{
		return m_frames[ _frame ].size;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageMNG::getFrameOffset( size_t _frame )
	{
		return m_images[ m_frames[ _frame ].index ].offset;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface * ResourceImageMNG::getFrameImage( size_t _frame )
	{
		return m_images[ m_frames[ _frame ].index ].renderImage;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageMNG::loader( TiXmlElement * _xml )
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

			m_images.push_back(imageProps);
		}

		size = desc.frames.size();

		m_frames.resize(size);

		for(size_t i = 0; i < size; i++)
		{
			m_frames[i].index = desc.frames[i].index;
			m_frames[i].delay = desc.frames[i].delay;

			float width = m_images[ m_frames[i].index ].renderImage->getWidth();
			float height = m_images[ m_frames[i].index ].renderImage->getHeight();

			m_frames[i].size = mt::vec2f( width, height );
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