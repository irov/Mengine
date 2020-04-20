#include "Componentable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Componentable::Componentable()
        : m_componentFamily( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Componentable::~Componentable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Componentable::addComponent( const ComponentPtr & _component )
    {
        uint32_t id = _component->getComponentId();

        m_components[id] = _component;

        m_componentFamily |= 1 << id;
    }
    //////////////////////////////////////////////////////////////////////////
    void Componentable::removeComponentId( uint32_t id )
    {
        m_components[id] = nullptr;

        m_componentFamily &= ~(1 << id);
    }
}