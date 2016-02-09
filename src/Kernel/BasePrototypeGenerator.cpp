#	include "BasePrototypeGenerator.h"

#   include "Interface/ScriptSystemInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	BasePrototypeGenerator::BasePrototypeGenerator()
		: m_serviceProvider( nullptr )
		, m_scriptWrapper( nullptr )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void BasePrototypeGenerator::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * BasePrototypeGenerator::getServiceProvider() const
	{
		return m_serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////	
	bool BasePrototypeGenerator::initialize( const ConstString & _category, const ConstString & _prototype )
	{
		m_category = _category;
		m_prototype = _prototype;

		m_scriptWrapper = SCRIPT_SERVICE( m_serviceProvider )
			->getWrapper( m_prototype );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void BasePrototypeGenerator::destroy()
	{
		delete this;
	}
}