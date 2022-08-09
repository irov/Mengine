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
        FactoryInterfacePtr factory = this->_initializeFactory();

        MENGINE_ASSERTION_MEMORY_PANIC( factory );

        m_prototypeFactory = factory;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryPrototypeGenerator::finalize()
    {
        this->_finalizeFactory();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_prototypeFactory );
        m_prototypeFactory = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t FactoryPrototypeGenerator::count() const
    {
        uint32_t count = m_prototypeFactory->getCountObject();

        return count;
    }
    //////////////////////////////////////////////////////////////////////////
}