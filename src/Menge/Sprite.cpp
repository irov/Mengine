#	include "Sprite.h"
#	include "ObjectImplement.h"

#	include "RenderEngine.h"

#	include "Manager/XmlManager.h"

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT(Sprite);
//////////////////////////////////////////////////////////////////////////
Sprite::Sprite()
: m_spriteDesc(0)
, m_image(0)
{

}
//////////////////////////////////////////////////////////////////////////
void Sprite::render()
{
	const mt::mat3f &wm = getWorldMatrix();
	m_renderEngine->renderImage(wm,0xffffffff,m_image);
}
//////////////////////////////////////////////////////////////////////////
bool Sprite::_compile()
{
	if( m_spriteDesc == 0 )
	{
		return false;
	}

	m_image = m_renderEngine->loadImage(m_spriteDesc->imageFile,m_spriteDesc->haveAlpha);

	delete m_spriteDesc;

	return ( m_image != 0);
}
//////////////////////////////////////////////////////////////////////////
void Sprite::_release()
{
	m_renderEngine->releaseRenderImage(m_image);
}
//////////////////////////////////////////////////////////////////////////
void Sprite::loader(TiXmlElement *xml)
{
	m_spriteDesc = new SpriteDesc;
	XML_FOR_EACH_ATTRIBUTES(xml)
	{
		XML_VALUE_ATTRIBUTE("File",m_spriteDesc->imageFile);
		XML_VALUE_ATTRIBUTE("HaveAlpha",m_spriteDesc->haveAlpha);
	}
}
//////////////////////////////////////////////////////////////////////////
void Sprite::_debugRender()
{
	const mt::vec2f &pos = getWorldPosition();
	const mt::vec2f &dir = getWorldDirection();



	m_renderEngine->drawLine(pos - dir*4, pos + dir*4 , 4 , 0xff00ff00 );
};