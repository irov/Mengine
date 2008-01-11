#	include "XmlElementListener.h"

//////////////////////////////////////////////////////////////////////////
XmlElementListener::XmlElementListener()
	: m_reference(0)
{
}

//////////////////////////////////////////////////////////////////////////
XmlElementListener::~XmlElementListener()
{
}
//////////////////////////////////////////////////////////////////////////
void XmlElementListener::incref()
{
	++m_reference;
}
//////////////////////////////////////////////////////////////////////////
bool XmlElementListener::decref()
{
	return --m_reference == 0;
}