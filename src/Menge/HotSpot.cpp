#	include "HotSpot.h"
#	include "ObjectImplement.h"

#	include "Manager/XmlManager.h"

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT(HotSpot)
//////////////////////////////////////////////////////////////////////////
void HotSpot::addPoint( const mt::vec2f &p )
{
	m_listPoint.push_back(p);
}
//////////////////////////////////////////////////////////////////////////
bool HotSpot::testPoint( const mt::vec2f &p )
{
	return true;
}
//////////////////////////////////////////////////////////////////////////
void HotSpot::loader( TiXmlElement *_xml)
{
	XML_FOR_EACH_TREE( _xml )
	{
		XML_CHECK_NODE("POINT")
		{
			mt::vec2f point;
			XML_VALUE_ATTRIBUTE("VALUE", point);
			addPoint(point);
		}
	}
};