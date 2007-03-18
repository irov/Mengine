#	include "Sprite.h" 

#	include "ObjectImplement.h"

#	include "RenderEngine.h"

#	include "XmlParser.h"

#	include "RenderSystemInterface.h"

#	include "FileSystemInterface.h"

#	include "FileEngine.h"

#	include "../MngReader/MNG.h"

#	include "math/bv.h"

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT(Sprite);
//////////////////////////////////////////////////////////////////////////
Sprite::Sprite()
: m_playing(false)
, m_looping(true)
, m_state(FORWARD)
, m_ctdelay(0.f)
, m_offset(0.f,0.f)
, m_size(0.f,0.f)
{
}

///////////////////////////////////////////////////////////////////////////
bool Sprite::isVisible(const Viewport & _viewPort)
{
	const mt::vec2f& pos = getLocalPosition();
	const mt::mat3f &wm = getWorldMatrix();

	mt::vec3f min0, max0;

	calculate_aabb_from_obb(min0,max0,getLocalPosition()+m_offset,m_size,getWorldMatrix());

	if (max0.x < _viewPort.begin.x || min0.x > _viewPort.end.x ) return false;
	if (max0.y < _viewPort.begin.y || min0.y > _viewPort.end.y ) return false;

//	Holder<RenderEngine>::hostage()->drawLine(min0.v2,max0.v2,2,0xffff00ff);

	return true;
}
//////////////////////////////////////////////////////////////////////////
void Sprite::setLooped(bool _looped)
{
	m_looping = _looped;
}
//////////////////////////////////////////////////////////////////////////
bool Sprite::getLooped() const
{
	return m_looping;
}
//////////////////////////////////////////////////////////////////////////
void Sprite::setFirstFrame()
{
	assert(m_state == FORWARD || m_state == REWIND);

	m_currentFrame = 
		(m_state == FORWARD)
		? m_frames.begin() 
		: m_frames.end() - 1;
}
//////////////////////////////////////////////////////////////////////////
void Sprite::setOffset(const mt::vec2f& _offset)
{
	m_offset = _offset;
}
//////////////////////////////////////////////////////////////////////////
void Sprite::_update(float _timing)
{
	assert(m_state == FORWARD || m_state == REWIND);
	
	if(!m_playing)
	{
		return; 
	}

	m_ctdelay += _timing;

	int delay = m_currentFrame->delay;

	while(m_ctdelay >= delay)
	{
		m_ctdelay -= delay;

		switch(m_state)
		{
			case FORWARD:
			{
				if(++m_currentFrame == m_frames.end())
				{
					if(!m_looping)
					{
						m_playing = false;
						m_currentFrame = m_frames.end() - 1;
						break;
					}
					else
					{
						m_currentFrame = m_frames.begin();
					}
				}	
			}
			break;

			case REWIND:
			{
				if(m_currentFrame == m_frames.begin())
				{
					if(!m_looping)
					{
						m_playing = false;
						m_currentFrame = m_frames.begin();
						break;
					}
					else
					{
						m_currentFrame = m_frames.end();
					}
				}
				--m_currentFrame;
			}
			break;

			default:
			{
				assert(!"undefined state!");
			}
			break;
		}
		delay = m_currentFrame->delay;
	}
}
//////////////////////////////////////////////////////////////////////////
bool Sprite::_compile()
{
	/*
	SpriteDecoder* mngdecoder =(SpriteDecoder*)Decoder::getDecodec(".mng");

	m_sprData = (SpriteDecoder::SpriteData*)mngdecoder->decode(fileData);
*/
	FileDataInterface* fileData = Holder<FileEngine>::hostage()->openFile(m_fileMNG);

	assert(fileData != 0);

	mnglib::mngDesc	m_desc;

	readMNG(
		m_desc,
		(unsigned char*)fileData->getBuffer(),
		fileData->size()
		);

	m_size.x = m_desc.width;
	m_size.y = m_desc.height;


	Holder<FileEngine>::hostage()->closeFile(fileData);

	TextureDesc	textureDesc;

	size_t size = m_desc.images.size();

	for(size_t i = 0; i < size; i++)
	{
		textureDesc.buffer = m_desc.images[i].buffer;
		textureDesc.size = m_desc.images[i].size;
		textureDesc.haveAlpha = true;

		Image	imageProps;

		imageProps.offset = mt::vec2f(
			(float)m_desc.images[i].offsetX, 
			(float)m_desc.images[i].offsetY);

		imageProps.renderImage = Holder<RenderEngine>::hostage()->loadImage(textureDesc);

		m_images.push_back(imageProps);
	}

	size = m_desc.frames.size();

	m_frames.resize(size);

	for(size_t i = 0; i < size; i++)
	{
		m_frames[i].index = m_desc.frames[i].index;
		m_frames[i].delay = m_desc.frames[i].delay;
	}

	setFirstFrame();

	freeMNG(m_desc);

	return true;
}
//////////////////////////////////////////////////////////////////////////
void Sprite::_release()
{
	size_t size = m_images.size();

	for(size_t i = 0; i < size; i++)
	{
		Holder<RenderEngine>::hostage()->releaseRenderImage(m_images[i].renderImage);
	}
}
//////////////////////////////////////////////////////////////////////////
void Sprite::loader(TiXmlElement *xml)
{
	XML_FOR_EACH_TREE(xml)
	{
		XML_CHECK_NODE("ImageMNG")
		{
			XML_VALUE_ATTRIBUTE("File", m_fileMNG);
		}		
	}

	Renderable::loader(xml);
}
//////////////////////////////////////////////////////////////////////////
void Sprite::stop()
{
	m_playing = false;
}
//////////////////////////////////////////////////////////////////////////
void Sprite::play()
{
	m_playing = true;
	setFirstFrame();
}
//////////////////////////////////////////////////////////////////////////
void Sprite::_render( const mt::mat3f &rwm, const Viewport & _viewPort )
{
	Holder<RenderEngine>::hostage()->renderImageOffset(
		rwm, 
		m_images[m_currentFrame->index].offset + m_offset,
		0xffffffff,
		m_images[m_currentFrame->index].renderImage
		);
}
//////////////////////////////////////////////////////////////////////////
void Sprite::_debugRender()
{

};



