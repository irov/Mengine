#	include "Sprite.h"
#	include "ObjectImplement.h"
#	include "RenderEngine.h"
#	include "Manager/XmlManager.h"

/*	непонятно как тут достать TextureDesc, поэтому добавил инклуд	*/
#	include "RenderSystemInterface.h"
#	include "FileSystemInterface.h"
#	include "FileEngine.h"
/*	конец*/

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT(Sprite);
//////////////////////////////////////////////////////////////////////////
Sprite::Sprite()
: m_playing(true)
, m_looping(true)
, m_haveAlpha(true)
{
	m_currentDelay = 0;
	m_state = FORWARD;  
}
//////////////////////////////////////////////////////////////////////////
void Sprite::render()
{
	const mt::mat3f& wm = getWorldMatrix();

	Keeper<RenderEngine>::hostage()->renderImage(
		wm,
		0xffffffff,
		m_renderImages[m_currentFrame->index]
	);
}
void Sprite::setLooped(bool flag)
{
	m_looping = flag;
}
//////////////////////////////////////////////////////////////////////////
bool Sprite::getLooped() const
{
	return m_looping;
}
//////////////////////////////////////////////////////////////////////////
void Sprite::update(float timing)
{
	if(!m_playing)
	{
		return; 
	}

	m_currentDelay+=timing;

	float delay = m_currentFrame->delay;

	while(m_currentDelay >= delay)
	{
		m_currentDelay -= delay;

		if(m_state == FORWARD)
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

		if(m_state == REWIND)
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

		delay = m_currentFrame->delay;
	}
}
//////////////////////////////////////////////////////////////////////////
bool Sprite::_compile()
{
	FileDataInterface* fileData = Keeper<FileEngine>::hostage()->openFile(m_fileMNG);

	readMNG(
		m_desc,
		(unsigned char*)fileData->getBuffer(),
		fileData->size()
		);

	Keeper<FileEngine>::hostage()->closeFile(fileData);

	TextureDesc	textureDesc;

	for(int i = 0; i < m_desc.images.size(); i++)
	{
		textureDesc.buffer = m_desc.images[i].buffer;
		textureDesc.size = m_desc.images[i].size;
		textureDesc.haveAlpha = true;
		m_renderImages.push_back(
			Keeper<RenderEngine>::hostage()->loadImage(textureDesc)
			);
	}

	m_currentFrame = (m_state == FORWARD) ? m_desc.frames.begin() : m_desc.frames.end() - 1;
	return true;
}
//////////////////////////////////////////////////////////////////////////
void Sprite::_release()
{
	for(int i = 0; i < m_desc.images.size(); i++)
	{
		Keeper<RenderEngine>::hostage()->releaseRenderImage(m_renderImages[i]);
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
}
//////////////////////////////////////////////////////////////////////////
void Sprite::play()
{
	m_playing = true;
	m_currentFrame = (m_state == FORWARD) ? m_desc.frames.begin() : m_desc.frames.end() - 1;
}

//////////////////////////////////////////////////////////////////////////
void Sprite::_debugRender()
{
	/*const mt::vec2f &pos = getWorldPosition();
	const mt::vec2f &dir = getWorldDirection();

	Keeper<RenderEngine>::hostage()->drawLine(pos - dir*4, pos + dir*4 , 4 , 0xff00ff00 );
	*/
	render();
};