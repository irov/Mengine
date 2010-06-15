#	include "Identity.h"

#	include "Factory/FactoryIdentity.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Identity::Identity()
		: m_factoryIdentity(0)
		, m_nameIdentity(-1)
		, m_typeIdentity(-1)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Identity::setFactoryIdentity( FactoryIdentity * _factoryIdentity )
	{
		m_factoryIdentity = _factoryIdentity;
	}
	//////////////////////////////////////////////////////////////////////////
	void Identity::setName(const String & _name)
	{
		m_nameIdentity = m_factoryIdentity->cacheIdentity( _name );
	}
	//////////////////////////////////////////////////////////////////////////
	const String & Identity::getName() const
	{
		return m_factoryIdentity->getIdentity( m_nameIdentity );
	}
	//////////////////////////////////////////////////////////////////////////
	void Identity::setType( const String & _type)
	{
		m_typeIdentity = m_factoryIdentity->cacheIdentity( _type );
	}
	//////////////////////////////////////////////////////////////////////////
	const String & Identity::getType() const
	{
		return m_factoryIdentity->getIdentity( m_typeIdentity );
	}
}
