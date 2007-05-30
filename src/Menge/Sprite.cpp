#	include "Sprite.h" 

#	include "ObjectImplement.h"

#	include "RenderEngine.h"

#	include "XmlParser.h"

#	include "RenderSystemInterface.h"

#	include "FileSystemInterface.h"

#	include "ResourceImage.h"

#	include "ResourceManager.h"

#	include "math/bv.h"

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT(Sprite);
//////////////////////////////////////////////////////////////////////////
Sprite::Sprite()
: m_playing(false)
, m_looping(true)
, m_state(FORWARD)
, m_total_delay(0.f)
, m_offset(0.f,0.f)
, m_currentFrame(0)
{}
///////////////////////////////////////////////////////////////////////////
bool Sprite::isVisible(const Viewport & _viewPort)
{
	const mt::vec2f& pos = getWorldPosition();
	const mt::mat3f &wm = getWorldMatrix();

	mt::vec3f min0, max0;

	const mt::vec2f & size = m_image->getMaxSize();

	const mt::vec2f & localPosition = getLocalPosition();
	const mt::mat3f & worldMatrix = getWorldMatrix();

	calculate_aabb_from_obb(
		min0,
		max0,
		localPosition+m_offset,
		size,
		worldMatrix);

	if (max0.x < _viewPort.begin.x || min0.x > _viewPort.end.x ) return false;
	if (max0.y < _viewPort.begin.y || min0.y > _viewPort.end.y ) return false;

	Holder<RenderEngine>::hostage()->drawLine(min0.v2,max0.v2,2,0xffff00ff);

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

	size_t frameSize = m_image->getFrameCount();

	m_currentFrame = 
		(m_state == FORWARD)
		? 0
		: frameSize - 1;
}
//////////////////////////////////////////////////////////////////////////
void Sprite::setOffset(const mt::vec2f& _offset)
{
	m_offset = _offset;
}
//////////////////////////////////////////////////////////////////////////
void Sprite::nextFrame()
{
	size_t frimeSize = m_image->getFrameCount();

	if( ++m_currentFrame == frimeSize )
	{
		if( m_looping == false )
		{
			m_playing = false;
			m_currentFrame = frimeSize - 1;
			return;
		}
		else
		{
			m_currentFrame = 0;
		}
	}	
}
//////////////////////////////////////////////////////////////////////////
void Sprite::prevFrame()
{
	size_t frimeSize = m_image->getFrameCount();

	if( m_currentFrame == 0 )
	{
		if(!m_looping)
		{
			m_playing = false;
			m_currentFrame = 0;
			return;
		}
		else
		{
			m_currentFrame = frimeSize;
		}
	}
	--m_currentFrame;
}
//////////////////////////////////////////////////////////////////////////
void Sprite::_update(float _timing)
{
	assert(m_state == FORWARD || m_state == REWIND);
	
	if(!m_playing)
	{
		return; 
	}

	m_total_delay += _timing;

	int delay = m_image->getFrameDelay( m_currentFrame );

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
		delay = m_image->getFrameDelay( m_currentFrame );
	}
}
//////////////////////////////////////////////////////////////////////////
bool Sprite::_activate()
{
	m_image = 
		Holder<ResourceManager>::hostage()
		->getResourceT<ResourceImage>( m_resourceName );

	if( m_image == 0 )
	{
		return false;
	}

	setFirstFrame();
	
	return true;
}
//////////////////////////////////////////////////////////////////////////
void Sprite::_deactivate()
{
	Holder<ResourceManager>::hostage()
		->releaseResource( m_image );
}
//////////////////////////////////////////////////////////////////////////
void Sprite::loader(TiXmlElement *xml)
{
	XML_FOR_EACH_TREE(xml)
	{
		XML_CHECK_NODE("Resource")
		{
			XML_VALUE_ATTRIBUTE("Name", m_resourceName);
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
	const mt::vec2f & size = m_image->getFrameSize( m_currentFrame );
	const mt::vec2f & image_offset = m_image->getFrameOffset( m_currentFrame );
	const mt::vec4f & frame_uv = m_image->getFrameUV( m_currentFrame );
	
	RenderImageInterface * renderImage = m_image->getFrameImage( m_currentFrame );

	Holder<RenderEngine>::hostage()->renderImage(
		rwm, 
		image_offset + m_offset,
		frame_uv,
		size,
		0xffffffff,
		renderImage
		);
}
//////////////////////////////////////////////////////////////////////////
void Sprite::_debugRender()
{};



