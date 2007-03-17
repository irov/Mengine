#	include "SoundNode.h"

#	include "ObjectImplement.h"

#	include "SoundSystemInterface.h"
#	include "SoundEngine.h"

#	include "XmlParser.h"
#	include "FileEngine.h"

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT(SoundNode);
//////////////////////////////////////////////////////////////////////////
SoundNode::SoundNode()
: m_interface(NULL)
{}
//////////////////////////////////////////////////////////////////////////
bool		SoundNode::play()
{
	return 0;//m_interface->play();
}
//////////////////////////////////////////////////////////////////////////
bool		SoundNode::pause()
{
	return 0;//m_interface->pause();
}
//////////////////////////////////////////////////////////////////////////
bool		SoundNode::stop()
{
	return 0;//m_interface->stop();
}
//////////////////////////////////////////////////////////////////////////
void		SoundNode::setLoop(bool _flag)
{
	//m_interface->setLoop(_flag);
}
//////////////////////////////////////////////////////////////////////////
bool		SoundNode::isLooping() const
{
	return	0;//m_interface->isLooping();
}
//////////////////////////////////////////////////////////////////////////
void		SoundNode::setGain(float _value)
{
//	m_interface->setGain(_value);
}
//////////////////////////////////////////////////////////////////////////
float		SoundNode::getGain() const
{
	return	0;//m_interface->getGain();
}
//////////////////////////////////////////////////////////////////////////
void		SoundNode::setPosition(const mt::vec3f& _position)
{
	m_interface->setPosition(_position.x,_position.y,_position.z);
}
//////////////////////////////////////////////////////////////////////////
mt::vec3f	SoundNode::getPosition() const
{
	const float* pos = m_interface->getPosition();
	return mt::vec3f(pos[0],pos[1],pos[2]);
}
//////////////////////////////////////////////////////////////////////////
void		SoundNode::setRelativeToListener(bool flag)
{
//	m_interface->setRelativeToListener(flag);
}
//////////////////////////////////////////////////////////////////////////
bool		SoundNode::isRelativeToListener() const
{
	return	0;//m_interface->isRelativeToListener();
}
//////////////////////////////////////////////////////////////////////////
bool		SoundNode::isPlaying() const
{
	return	0;//m_interface->isPlaying();
}
//////////////////////////////////////////////////////////////////////////
double		SoundNode::getSizeSec()	const
{
	return	0;//m_interface->getSizeSec();
}
//////////////////////////////////////////////////////////////////////////
void	SoundNode::_update(float _timing)
{
	//m_interface->process();
}
//////////////////////////////////////////////////////////////////////////
void		SoundNode::loader(TiXmlElement * xml)
{
	XML_FOR_EACH_TREE(xml)
	{
		XML_CHECK_VALUE_NODE("Filename","Value",m_filename);
	}
}
//////////////////////////////////////////////////////////////////////////
bool		SoundNode::_compile()
{
	Holder<SoundEngine>::hostage()->addSoundNode(m_interface,m_fileData,m_filename,false);
	//play(); //for test
	return	true;
}
//////////////////////////////////////////////////////////////////////////
void		SoundNode::_release()
{
	Holder<SoundEngine>::hostage()->deleteSound(m_interface);
	Holder<FileEngine>::hostage()->closeFile(m_fileData);
}
