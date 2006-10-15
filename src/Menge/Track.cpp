#	include "Track.h"
#	include "ObjectImplement.h"

#	include "TrackProxy.h"

#	include "Manager/XmlManager.h"

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT(Track)
//////////////////////////////////////////////////////////////////////////
void Track::addPoint(const mt::vec2f &_point)
{
	if( m_listPoints.empty() )
	{
		m_listPoints.push_back(_point);
	}
	else
	{
		mt::vec2f &begin = m_listPoints.back();
		m_listPoints.push_back(_point);
		mt::vec2f &end = m_listPoints.back();

		m_listChain.push_back(TrackChain(&begin,&end));
	}
}
//////////////////////////////////////////////////////////////////////////
bool Track::addChildren(Node *_node) 
{
	if( dynamic_cast<Allocator*>(_node) == 0 )
	{
		return false;
	}

	TrackProxy *proxy = new TrackProxy(_node,this);

	proxy->setChain(m_listChain.begin());

	return Node::addChildren(proxy);
};
//////////////////////////////////////////////////////////////////////////
void Track::updateTrackNode(TrackProxy *_trackProxy, float _timing)
{
	float speedMove = _trackProxy->getSpeedMove();
	float trackLength = speedMove * _timing;

	const mt::vec2f &lposition = 
		_trackProxy->getLocalPosition();

	TListTrackChain::iterator it = 
		_trackProxy->getChain();

	float lnChain = mt::length_v2_v2(lposition, it->endPoint());

	while( trackLength > lnChain )
	{
		trackLength -= lnChain;

		++it;

		if( it == m_listChain.end() )
		{
			TrackChain &backChain = m_listChain.back();

			const mt::vec2f &endPoint = backChain.endPoint();
			_trackProxy->setPosition(endPoint);

			_trackProxy->stop();
			break;
		}

		lnChain = it->length();

		if( trackLength < lnChain )
		{
			_trackProxy->setChain(it);
			break;
		}		
	}

	if( _trackProxy->isMoved() == true )
	{	

		const mt::vec2f &dir = it->direction();

		mt::vec2f pos = _trackProxy->getLocalPosition();

		pos += dir * trackLength;

		_trackProxy->setDirection(dir);
		_trackProxy->setPosition(pos);
	}
}
//////////////////////////////////////////////////////////////////////////
void Track::loader(TiXmlElement *_xml)
{
	XML_FOR_EACH_TREE(_xml)
	{
		XML_CHECK_NODE_FOR_EACH("Points")
		{
			XML_CHECK_NODE("Point")
			{
				mt::vec2f pos;
				XML_VALUE_ATTRIBUTE("Value",pos);
				addPoint(pos);
			}
		}
	}

	Node::loader(_xml);
};
//////////////////////////////////////////////////////////////////////////
#	include "SceneManager.h"
#	include "RenderEngine.h"
//////////////////////////////////////////////////////////////////////////
void Track::_debugRender()
{
	RenderEngine *renderEng = Keeper<RenderEngine>::hostage();
 
	for( TListTrackChain::iterator 
		it = m_listChain.begin(),
		it_end = m_listChain.end();
	it != it_end;
	++it)
	{
		renderEng->drawLine(it->beginPoint(),it->endPoint(),2,0xffff00ff);
	}
};