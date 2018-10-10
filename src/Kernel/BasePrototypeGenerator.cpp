#include "BasePrototypeGenerator.h"

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
    void BasePrototypeGenerator::setCategory( const ConstString & _category )
    {
        m_category = _category;
    }
    //////////////////////////////////////////////////////////////////////////
    void BasePrototypeGenerator::setPrototype( const ConstString & _prototype )
    {
        m_prototype = _prototype;
    }
    //////////////////////////////////////////////////////////////////////////	
    bool BasePrototypeGenerator::initialize()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t BasePrototypeGenerator::count() const
    {
        //Empty

        return 0U;
    }
}