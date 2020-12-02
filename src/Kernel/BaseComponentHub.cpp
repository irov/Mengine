#include "BaseComponentHub.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BaseComponentHub::BaseComponentHub()
        : m_componentFamily( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    BaseComponentHub::~BaseComponentHub()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseComponentHub::addComponent( const ComponentInterfacePtr & _component )
    {
        uint32_t id = _component->getComponentId();

        m_components[id] = _component;

        m_componentFamily |= 1 << id;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseComponentHub::removeComponentById( uint32_t id )
    {
        m_components[id] = nullptr;

        m_componentFamily &= ~(1 << id);
    }
    //////////////////////////////////////////////////////////////////////////
}