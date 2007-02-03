#	include "SpriteInteractive.h"
#	include "ObjectImplement.h"

#	include "XmlParser.h"

OBJECT_IMPLEMENT(SpriteInteractive)
//////////////////////////////////////////////////////////////////////////
void SpriteInteractive::stop()
{
	Sprite::stop();
	m_playSound.stop();
}
//////////////////////////////////////////////////////////////////////////
void SpriteInteractive::play()
{
	Sprite::play();
	m_playSound.play();
}
//////////////////////////////////////////////////////////////////////////
void SpriteInteractive::setLooped(bool _looped)
{
	Sprite::setLooped(_looped);
	m_playSound.setLoop(_looped);
}
//////////////////////////////////////////////////////////////////////////
void SpriteInteractive::loader(TiXmlElement *_xml)
{
	Sprite::loader(_xml);
	m_playSound.loader(_xml);
}