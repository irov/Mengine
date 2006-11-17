#	include "Sprite.h"
#	include "ObjectImplement.h"
#	include "RenderEngine.h"
#	include "XmlReader.h"
#	include "RenderSystemInterface.h"
#	include "FileSystemInterface.h"
#	include "FileEngine.h"
//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT(Sprite);
//////////////////////////////////////////////////////////////////////////
Sprite::Sprite()
: m_playing(true)
, m_looping(true)
, m_state(REWIND)
, m_ctdelay(0)
{}
//////////////////////////////////////////////////////////////////////////
void Sprite::_render()
{
	const mt::mat3f& wm = getWorldMatrix();
	
	Keeper<RenderEngine>::hostage()->renderImageOffset(
		wm, 
		m_images[m_currentFrame->index].offset,
		0xffffffff,
		m_images[m_currentFrame->index].renderImage
	);
}
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
		? m_desc.frames.begin() 
		: m_desc.frames.end() - 1;
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
				++m_currentFrame;

				if(m_currentFrame == m_desc.frames.end())
				{
					if(!m_looping)
					{
						m_playing = false;
						m_currentFrame = m_desc.frames.end() - 1;
						break;
					}
					else
					{
						m_currentFrame = m_desc.frames.begin();
					}
				}	
			}
			break;

			case REWIND:
			{
				if(m_currentFrame == m_desc.frames.begin())
				{
					if(!m_looping)
					{
						m_playing = false;
						m_currentFrame = m_desc.frames.begin();
						break;
					}
					else
					{
						m_currentFrame = m_desc.frames.end();
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
	//	open *.mng from *.pak:
	FileDataInterface* fileData = Keeper<FileEngine>::hostage()->openFile(m_fileMNG);

	assert(fileData != 0);

	//	read *.mng format in m_desc structure:
	readMNG(
		m_desc,
		(unsigned char*)fileData->getBuffer(),
		fileData->size()
		);

	//	release fileData:
	Keeper<FileEngine>::hostage()->closeFile(fileData);

	//	fill internal structures:
	TextureDesc	textureDesc;

	size_t size = m_desc.images.size();
	for(size_t i = 0; i < size; i++)
	{
		textureDesc.buffer = m_desc.images[i].buffer;
		textureDesc.size = m_desc.images[i].size;
		textureDesc.haveAlpha = true;

		ImageProperties	ip;
		ip.offset = mt::vec2f(
			(float)m_desc.images[i].offsetX, 
			(float)m_desc.images[i].offsetY);
		ip.renderImage = Keeper<RenderEngine>::hostage()->loadImage(textureDesc);

		m_images.push_back(ip);
	}

	//	set first frame:
	setFirstFrame();
	return true;
}
//////////////////////////////////////////////////////////////////////////
void Sprite::_release()
{
	size_t size = m_desc.images.size();

	for(size_t i = 0; i < size; i++)
	{
		Keeper<RenderEngine>::hostage()->releaseRenderImage(m_images[i].renderImage);
	}

	freeMNG(m_desc);
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
	setFirstFrame();
}
//////////////////////////////////////////////////////////////////////////
void Sprite::play()
{
	m_playing = true;
}
//////////////////////////////////////////////////////////////////////////
void Sprite::pause()
{
	m_playing = false;
}
//////////////////////////////////////////////////////////////////////////
void Sprite::_debugRender()
{
	const mt::mat3f& wm = getWorldMatrix();

	Keeper<RenderEngine>::hostage()->renderImageOffset(
		wm, 
		m_images[m_currentFrame->index].offset,
		0xffffffff,
		m_images[m_currentFrame->index].renderImage
		);

};