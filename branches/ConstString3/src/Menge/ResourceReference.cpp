#	include "ResourceReference.h"
#	include "Factory/FactoryIdentity.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ResourceReference::ResourceReference()
		: m_factoryIdentity(0)
		, m_nameIdentity(-1)
		, m_categoryIdentity(-1)
		, m_groupIdentity(-1)
		, m_fileIdentity(-1)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceReference::~ResourceReference()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceReference::initialize( const ResourceFactoryParam & _params, FactoryIdentity * _factoryIdentity )
	{
		m_factoryIdentity = _factoryIdentity;

		m_nameIdentity = m_factoryIdentity->cacheIdentity( _params.name );
		m_categoryIdentity = m_factoryIdentity->cacheIdentity( _params.category );
		m_groupIdentity = m_factoryIdentity->cacheIdentity( _params.group );
		m_fileIdentity = m_factoryIdentity->cacheIdentity( _params.file );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceReference::initialize( const ResourceFactoryIdentity & _identities, FactoryIdentity * _factoryIdentity )
	{
		m_factoryIdentity = _factoryIdentity;

		m_nameIdentity = _identities.name;
		m_categoryIdentity = _identities.category;
		m_groupIdentity = _identities.group;
		m_fileIdentity = _identities.file;
	}
	//////////////////////////////////////////////////////////////////////////
	const String& ResourceReference::getName() const
	{
		return m_factoryIdentity->getIdentity( m_nameIdentity );
	}
	//////////////////////////////////////////////////////////////////////////
	const String & ResourceReference::getCategory() const
	{
		return m_factoryIdentity->getIdentity( m_categoryIdentity );
	}
	//////////////////////////////////////////////////////////////////////////
	const String & ResourceReference::getGroup() const
	{
		return m_factoryIdentity->getIdentity( m_groupIdentity );
	}
	//////////////////////////////////////////////////////////////////////////
	const String & ResourceReference::getFile() const
	{
		return m_factoryIdentity->getIdentity( m_fileIdentity );
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ResourceReference::getNameIdentity() const
	{
		return m_nameIdentity;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ResourceReference::getCategoryIdentity() const
	{
		return m_categoryIdentity;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ResourceReference::getGroupIdentity() const
	{
		return m_groupIdentity;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ResourceReference::getFileIdentity() const
	{
		return m_fileIdentity;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceReference::loader(XmlElement * _xml)
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceReference::parser( BinParser * _parser )
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceReference::_incrementZero()
	{
		bool result = this->compile();
		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceReference::_decrementZero()
	{
		this->release();
	}
	//////////////////////////////////////////////////////////////////////////
}
