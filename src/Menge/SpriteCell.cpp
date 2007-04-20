#	include "SpriteCell.h"

#	include "ObjectImplement.h"

#	include "RenderEngine.h"

#	include "XmlParser.h"

#	include "RenderSystemInterface.h"

#	include "FileSystemInterface.h"

#	include "FileEngine.h"

#	include "../MngReader/MNG.h"

#	include "math/bv.h"

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT(SpriteCell);
//////////////////////////////////////////////////////////////////////////
SpriteCell::SpriteCell()
: m_playing(false)
, m_looping(true)
, m_state(FORWARD)
, m_total_delay(0.f)
, m_offset(0.f,0.f)
, m_size(0.f,0.f)
{}
///////////////////////////////////////////////////////////////////////////
bool SpriteCell::isVisible(const Viewport & _viewPort)
{
	const mt::vec2f& pos = getWorldPosition();
	const mt::mat3f &wm = getWorldMatrix();

	mt::vec3f min0, max0;

	calculate_aabb_from_obb(min0,max0,getLocalPosition()+m_offset,m_size,getWorldMatrix());

	if (max0.x < _viewPort.begin.x || min0.x > _viewPort.end.x ) return false;
	if (max0.y < _viewPort.begin.y || min0.y > _viewPort.end.y ) return false;

	Holder<RenderEngine>::hostage()->drawLine(min0.v2,max0.v2,2,0xffff00ff);

	return true;
}
//////////////////////////////////////////////////////////////////////////
void SpriteCell::setLooped(bool _looped)
{
	m_looping = _looped;
}
//////////////////////////////////////////////////////////////////////////
bool SpriteCell::getLooped() const
{
	return m_looping;
}
//////////////////////////////////////////////////////////////////////////
void SpriteCell::setFirstFrame()
{
	assert(m_state == FORWARD || m_state == REWIND);

	m_currentFrame = 
		(m_state == FORWARD)
		? frames.begin() 
		: frames.end() - 1;
}
//////////////////////////////////////////////////////////////////////////
void SpriteCell::setOffset(const mt::vec2f& _offset)
{
	m_offset = _offset;
}
//////////////////////////////////////////////////////////////////////////
void SpriteCell::nextFrame()
{
	if(++m_currentFrame == frames.end())
	{
		if(!m_looping)
		{
			m_playing = false;
			m_currentFrame = frames.end() - 1;
			return;
		}
		else
		{
			m_currentFrame = frames.begin();
		}
	}	
}
//////////////////////////////////////////////////////////////////////////
void SpriteCell::prevFrame()
{
	if(m_currentFrame == frames.begin())
	{
		if(!m_looping)
		{
			m_playing = false;
			m_currentFrame = frames.begin();
			return;
		}
		else
		{
			m_currentFrame = frames.end();
		}
	}
	--m_currentFrame;
}
//////////////////////////////////////////////////////////////////////////
void SpriteCell::_update(float _timing)
{
	assert(m_state == FORWARD || m_state == REWIND);
	
	if(!m_playing)
	{
		return; 
	}

	m_total_delay += _timing;

	int delay = m_currentFrame->delay;

	while(m_total_delay >= delay)
	{
		m_total_delay -= delay;

		switch(m_state)
		{
			case FORWARD:
			{
				nextFrame();
			}
			break;

			case REWIND:
			{
				prevFrame();
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
bool SpriteCell::_compile()
{
//	SpriteDecoder* mngdecoder = static_cast<SpriteDecoder*>(Decoder::getDecodec(".mng"));

	//m_sprData = (SpriteDecoder::SpriteData*)mngdecoder->decode(fileData);

	FileDataInterface* fileData = Holder<FileEngine>::hostage()->openFile(m_fileMNG);

	assert(fileData != 0);

	fileData->read_ints(&m_cellWidth,1);
	fileData->read_ints(&m_cellHeight,1);

	fileData->read_ints(&m_numX,1);
	fileData->read_ints(&m_numY,1);

	m_imageWidth = m_numX * m_cellWidth;  
	m_imageHeight = m_numY * m_cellHeight;

	int size = m_numX*m_numY;

	frames.resize(size);

	for(size_t i = 0; i < size; i++)
	{
		fileData->read_ints(&frames[i].index,1);
		fileData->read_ints(&frames[i].delay,1);
	}

	fileData->read_ints(&size,1);
	char* buffer = new char[size];
	fileData->read_chars(buffer,size);

	TextureDesc	textureDesc;
	textureDesc.buffer = buffer;
	textureDesc.size = size;
	textureDesc.haveAlpha = false;

	Image	imageProps;

	imageProps.offset = mt::vec2f(0,0);
	imageProps.renderImage = Holder<RenderEngine>::hostage()->loadImage(textureDesc);
	images.push_back(imageProps);
	
	delete[] buffer;

	Holder<FileEngine>::hostage()->closeFile(fileData);
	setFirstFrame();
	return true;
}
//////////////////////////////////////////////////////////////////////////
void SpriteCell::_release()
{
	size_t size = images.size();

	for(size_t i = 0; i < size; i++)
	{
		Holder<RenderEngine>::hostage()->releaseRenderImage(images[i].renderImage);
	}
}
//////////////////////////////////////////////////////////////////////////
void SpriteCell::loader(TiXmlElement *xml)
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
void SpriteCell::stop()
{
	m_playing = false;
}
//////////////////////////////////////////////////////////////////////////
void SpriteCell::play()
{
	m_playing = true;
	setFirstFrame();
}
//////////////////////////////////////////////////////////////////////////
void SpriteCell::_render( const mt::mat3f &rwm, const Viewport & _viewPort )
{
	float u0 = float(m_currentFrame->index % m_numX) / m_numX;

	int offset = float(m_currentFrame->index) / m_numY;
	
	float v0 = float(offset)/m_numY;

	float u1 = float(m_currentFrame->index % m_numX + 1) / m_numX;

	float v1 = float(offset + 1) / m_numY;

	Holder<RenderEngine>::hostage()->renderImageUV(
		rwm, 0xffffffff,
		u0,v0,u1,v1,m_cellWidth,m_cellHeight,
		images[0].renderImage
		);
}
//////////////////////////////////////////////////////////////////////////
void SpriteCell::_debugRender()
{};