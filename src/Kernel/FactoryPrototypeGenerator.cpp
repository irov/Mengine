#include "FactoryPrototypeGenerator.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FactoryPrototypeGenerator::FactoryPrototypeGenerator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FactoryPrototypeGenerator::~FactoryPrototypeGenerator()
    {
    }
    //////////////////////////////////////////////////////////////////////////	
    bool FactoryPrototypeGenerator::initialize()
    {
        FactoryPtr factory = this->_initializeFactory();

        if( factory == nullptr )
        {
            return false;
        }

        m_factory = factory;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t FactoryPrototypeGenerator::count() const
    {
        uint32_t count = m_factory->getCountObject();

        return count;
    }
}