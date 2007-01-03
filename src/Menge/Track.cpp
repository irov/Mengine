#	include "Track.h"
#	include "ObjectImplement.h"

#	include "XmlParser.h"

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT(Track)
//////////////////////////////////////////////////////////////////////////
Track::Track()
: m_speedMove(0)
, m_isMove(true)
{
	
}
//////////////////////////////////////////////////////////////////////////
void Track::addPoint(const mt::vec3f &_point)
{
	if( m_listPoints.empty() )
	{
		m_listPoints.push_back(_point);
	}
	else
	{
		mt::vec3f &begin = m_listPoints.back();
		m_listPoints.push_back(_point);
		mt::vec3f &end = m_listPoints.back();

		m_listChain.push_back(TrackChain(&begin,&end));
	}
}
//////////////////////////////////////////////////////////////////////////
void Track::setSpeedMove(float _speed)
{
	m_speedMove = _speed;
}
//////////////////////////////////////////////////////////////////////////
float Track::getSpeedMove()const
{
	return m_speedMove;
}
//////////////////////////////////////////////////////////////////////////
void Track::start()
{
	m_isMove = true;
}
//////////////////////////////////////////////////////////////////////////
void Track::stop()
{
	m_isMove = false;
}
//////////////////////////////////////////////////////////////////////////
bool Track::isMoved()const
{
	return m_isMove;
}
//////////////////////////////////////////////////////////////////////////
bool Track::_compile()
{
	m_currentChain = m_listChain.begin();

	const mt::vec3f & bp = m_currentChain->beginPoint();

	setPosition(bp);

	return true;
}
//////////////////////////////////////////////////////////////////////////
void Track::_update( float _timing)
{
	if( m_listChain.empty() == true )
	{
		return;
	}

	if( m_isMove == false )
	{
		return;
	}	

	float trackLength = m_speedMove * _timing;

	const mt::vec3f &lposition = getLocalPosition();

	float lnChain = mt::length_v3_v3(lposition, m_currentChain->endPoint());

	while( trackLength > lnChain )
	{
		trackLength -= lnChain;

		++m_currentChain;

		if( m_currentChain == m_listChain.end() )
		{
			TrackChain &backChain = m_listChain.back();

			const mt::vec3f &endPoint = backChain.endPoint();
			setPosition(endPoint);

			stop();
			break;
		}

		lnChain = m_currentChain->length();

		if( trackLength < lnChain )
		{
			break;
		}		
	}

	if( m_isMove == true )
	{	
		const mt::vec3f &dir = m_currentChain->direction();

		mt::vec3f pos = getLocalPosition();

		pos += dir * trackLength;

		setDirection(dir);
		setPosition(pos);
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
				mt::vec3f pos;
				XML_VALUE_ATTRIBUTE("Value",pos);
				addPoint(pos);
			}
		}
		XML_CHECK_VALUE_NODE("SpeedMove","Value",m_speedMove);
	}

	NodeImpl::loader(_xml);
};
//////////////////////////////////////////////////////////////////////////
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
		//renderEng->drawLine(it->beginPoint(),it->endPoint(),2,0xffff00ff);
	}
};