#	include "TrackProxy.h"

#	include "Track.h"

#	include "Manager/XmlManager.h"

using namespace Menge;
//////////////////////////////////////////////////////////////////////////
TrackProxy::TrackProxy( Node *_node , Track *_track )
: NodeProxy(_node)
, m_track(_track)
, m_speedMove(1.f)
, m_isMove(true)
{
}
//////////////////////////////////////////////////////////////////////////
void TrackProxy::setDirection(const mt::vec2f &dir)
{
	Allocator *alloc = static_cast<Allocator*>(m_node);

	alloc->setDirection(dir);
}
//////////////////////////////////////////////////////////////////////////
void TrackProxy::setPosition(const mt::vec2f &pos)
{
	Allocator *alloc = static_cast<Allocator*>(m_node);

	alloc->setPosition(pos);
}
//////////////////////////////////////////////////////////////////////////
float TrackProxy::getSpeedMove()const
{
	return m_speedMove;
}
//////////////////////////////////////////////////////////////////////////
void TrackProxy::setChain(TListTrackChain::iterator _chain)
{
	m_chain = _chain;
}
//////////////////////////////////////////////////////////////////////////
TListTrackChain::iterator TrackProxy::getChain()
{
	return m_chain;
}
//////////////////////////////////////////////////////////////////////////
const mt::vec2f & TrackProxy::getLocalPosition()const
{
	Allocator *alloc = static_cast<Allocator*>(m_node);

	return alloc->getLocalPosition();
}
//////////////////////////////////////////////////////////////////////////
void TrackProxy::setSpeedMove(float _speed)
{
	m_speedMove = _speed;
}
//////////////////////////////////////////////////////////////////////////
void TrackProxy::update(float _timing)
{
	if( m_isMove )
	{
		m_track->updateTrackNode(this,_timing);
	}

	NodeProxy::update(_timing);
};
//////////////////////////////////////////////////////////////////////////
bool TrackProxy::_activate()
{
	const mt::vec2f &position = m_chain->beginPoint();

	setPosition(position);

	return NodeProxy::_activate();
}
//////////////////////////////////////////////////////////////////////////
void TrackProxy::start()
{
	m_isMove = true;
}
//////////////////////////////////////////////////////////////////////////
void TrackProxy::stop()
{
	m_isMove = false;
}
//////////////////////////////////////////////////////////////////////////
bool TrackProxy::isMoved()const
{
	return m_isMove;
}
//////////////////////////////////////////////////////////////////////////
void TrackProxy::loader(TiXmlElement * _xml)
{
	XML_FOR_EACH_TREE(_xml)
	{
		XML_CHECK_VALUE_NODE("SpeedMove","Value",m_speedMove);
	}

	NodeProxy::loader(_xml);
};