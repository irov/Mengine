#include "BasePrototypeGenerator.h"

#include "Interface/ScriptSystemInterface.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	BasePrototypeGenerator::BasePrototypeGenerator()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    BasePrototypeGenerator::~BasePrototypeGenerator()
    {
    }
	//////////////////////////////////////////////////////////////////////////	
	bool BasePrototypeGenerator::initialize( const ConstString & _category, const ConstString & _prototype )
	{
		m_category = _category;
		m_prototype = _prototype;

        FactoryPtr factory = this->_initializeFactory();

        if( factory == nullptr )
        {
            return false;
        }

        m_factory = factory;

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    uint32_t BasePrototypeGenerator::count() const
    {
        uint32_t count = m_factory->getCountObject();

        return count;
    }
	//////////////////////////////////////////////////////////////////////////
	void BasePrototypeGenerator::destroy()
	{
		delete this;
	}
}