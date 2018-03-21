#include "BasePrototypeGenerator.h"

#   include "Interface/ScriptSystemInterface.h"

namespace Mengine
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

		m_scriptWrapper = SCRIPT_SERVICE()
			->getWrapper( m_prototype );

        if( this->_initialize() == false )
        {
            return false;
        }

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    bool BasePrototypeGenerator::_initialize()
    {
        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void BasePrototypeGenerator::destroy()
	{
		delete this;
	}
}