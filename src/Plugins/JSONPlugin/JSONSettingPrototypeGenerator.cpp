#include "JSONSettingPrototypeGenerator.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JSONSettingPrototypeGenerator::JSONSettingPrototypeGenerator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONSettingPrototypeGenerator::~JSONSettingPrototypeGenerator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FactoryPtr JSONSettingPrototypeGenerator::_initializeFactory()
    {
        FactoryPtr factory = Helper::makeFactoryPool<JSONSetting, 128>();

        return factory;
    }
    //////////////////////////////////////////////////////////////////////////
    void JSONSettingPrototypeGenerator::_finalizeFactory()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    FactorablePointer JSONSettingPrototypeGenerator::generate( const Char * _doc )
    {
        const FactoryPtr & factory = this->getFactory();

        TypePtr setting = factory->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( setting, nullptr, "can't generate '%s:%s' doc '%s'"
            , this->getCategory().c_str()
            , this->getPrototype().c_str()
            , _doc
        );

        return setting;
    }

}