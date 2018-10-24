#include "FactoryPrototypeGenerator.h"
#include "FactoryAssertion.h"

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
    void FactoryPrototypeGenerator::finalize()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factory );
        m_factory = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t FactoryPrototypeGenerator::count() const
    {
        uint32_t count = m_factory->getCountObject();

        return count;
    }
}