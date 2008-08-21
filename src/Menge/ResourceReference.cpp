#	include "ResourceReference.h"
#	include "ProfilerEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ResourceReference::ResourceReference( const ResourceFactoryParam & _params )
	: m_params( _params )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceReference::~ResourceReference()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceReference::setName( const String& _name )
	{
		m_params.name = _name;
	}
	//////////////////////////////////////////////////////////////////////////
	const String& ResourceReference::getName()
	{
		return m_params.name;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceReference::loader(XmlElement * _xml)
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceReference::_incrementZero()
	{
		const String & name = this->getName();
		Holder<ProfilerEngine>::hostage()->addResourceToProfile(name);

		return compile();
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceReference::_decrementZero()
	{
		release();
	}
	//////////////////////////////////////////////////////////////////////////
	const ResourceFactoryParam& ResourceReference::getFactoryParams() const
	{
		return m_params;
	}
	//////////////////////////////////////////////////////////////////////////
}