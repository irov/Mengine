#	include "Identity.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	void Identity::setName(const ConstString & _name)
	{
		m_name = _name;
	}
	//////////////////////////////////////////////////////////////////////////
	void Identity::setType( const ConstString & _type)
	{
		m_type = _type;
	}
	//////////////////////////////////////////////////////////////////////////
	void Identity::setTag( const ConstString & _tag )
	{
		m_tag = _tag;
	}
}
