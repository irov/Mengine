#	include "BasePrototypeGenerator.h"

#   include "Interface/ScriptSystemInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	BasePrototypeGenerator::BasePrototypeGenerator()
		: m_scriptWrapper( nullptr )
	{
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