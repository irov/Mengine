#	include "SoundSystemInterface.h"
#	include "SoundSystemInterface.h"

#	include "SoundNode.h"

void intrusive_ptr_add_ref(Menge::SoundNode *_n)
{
	_n->addRefCnt();
}

void intrusive_ptr_release(Menge::SoundNode *_n)
{
	_n->releaseRefCnt();
}

namespace Menge
{
	SoundNode::SoundNode(SoundSystemInterface* _soundSystem, SoundSourceInterface *_interface)
		: m_ssytem(_soundSystem)
		, m_interface(_interface)
		, m_refCnt(0)
	{

	}

	SoundNode::~SoundNode()
	{
		m_ssytem->releaseSoundNode(m_interface);
	}

	void		SoundNode::play()
	{
		m_interface->play();
	}

	void		SoundNode::pause()
	{
		m_interface->pause();
	}

	void		SoundNode::stop()
	{
		m_interface->stop();
	}

	bool		SoundNode::update()
	{
		return m_interface->update();
	}

	void		SoundNode::setLooped(bool _flag)
	{
		m_interface->setLooped(_flag);
	}

	bool		SoundNode::getLooped() const
	{
		return	m_interface->getLooped();
	}

	void		SoundNode::setVolume(float _value)
	{
		m_interface->setVolume(_value);
	}

	float		SoundNode::getVolume() const
	{
		return	m_interface->getVolume();
	}

	void		SoundNode::setPosition(const float* _position)
	{
		m_interface->setPosition(_position);
	}

	const float*	SoundNode::getPosition() const
	{
		return m_interface->getPosition();
	}

	void		SoundNode::setDistance(float dist)
	{
		m_interface->setDistance(dist);
	}

	float		SoundNode::getDistance() const
	{
		return m_interface->getDistance();
	}

	void		SoundNode::setHeadMode(bool flag)
	{
		m_interface->setHeadMode(flag);
	}

	bool		SoundNode::getHeadMode() const
	{
		return	m_interface->getHeadMode();
	}

	void		SoundNode::addRefCnt()
	{
		++m_refCnt;
	}

	void		SoundNode::releaseRefCnt()
	{
		if(m_refCnt == 1)
		{
			delete this;
		}
		else
		{
			--m_refCnt;
		}
	}

	size_t		SoundNode::refCnt()
	{
		return m_refCnt;
	}
}