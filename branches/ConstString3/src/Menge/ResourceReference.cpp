#	include "ResourceReference.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ResourceReference::ResourceReference()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceReference::~ResourceReference()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceReference::initialize( const ResourceFactoryParam & _param )
	{
		m_param = _param;
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
	void ResourceReference::_loaded()
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
