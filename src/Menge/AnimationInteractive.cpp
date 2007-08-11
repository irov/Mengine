#	include "AnimationInteractive.h"

#	include "ObjectImplement.h"

#	include "XmlParser/XmlParser.h"

namespace	Menge
{
	////////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(AnimationInteractive)
	////////////////////////////////////////////////////////////////////////////
	void AnimationInteractive::stop()
	{
		Animation::stop();
		m_soundEmitter->stop();
	}
	////////////////////////////////////////////////////////////////////////////
	void AnimationInteractive::play()
	{
		Animation::play();
		m_soundEmitter->play();
	}
	////////////////////////////////////////////////////////////////////////////
	void AnimationInteractive::pause()
	{
		Animation::pause();
		m_soundEmitter->pause();
	}
	////////////////////////////////////////////////////////////////////////////
	void AnimationInteractive::setLooped(bool _looped)
	{
		Animation::setLooped(_looped);
		m_soundEmitter->setLooped(_looped);
	}
	////////////////////////////////////////////////////////////////////////////
	void AnimationInteractive::loader(TiXmlElement *_xml)
	{
		Animation::loader(_xml);
		m_soundEmitter->loader(_xml);
	}
}