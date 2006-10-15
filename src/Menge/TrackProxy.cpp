#	include "TrackProxy.h"

#	include "Track.h"

using namespace Menge;
//////////////////////////////////////////////////////////////////////////
TrackProxy::TrackProxy( const TrackProxyDesc &_desc )
: NodeProxy(_desc.node)
, m_track(_desc.track)
, m_chain(_desc.chain)
, m_speedMove(_desc.speedMove)
, m_isMove(true)
{
	const mt::vec2f &position = m_chain->beginPoint();

	setPosition(position);
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
