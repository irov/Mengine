#include "FactoryPrototypeGenerator.h"

#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"

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

        MENGINE_ASSERTION_MEMORY_PANIC( factory, false );

        m_factory = factory;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryPrototypeGenerator::finalize()
    {
        this->_finalizeFactory();

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