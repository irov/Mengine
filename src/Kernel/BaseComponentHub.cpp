#include "BaseComponentHub.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BaseComponentHub::BaseComponentHub()
        :m_mask( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    BaseComponentHub::~BaseComponentHub()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseComponentHub::setMask( uint32_t _mask )
    {
        m_mask = _mask;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t BaseComponentHub::getMask() const
    {
        return m_mask;
    }
    //////////////////////////////////////////////////////////////////////////
    const ComponentInterfacePtr & BaseComponentHub::getComponent( UniqueId _id ) const
    {
        const ComponentInterfacePtr & component = m_components.find( _id );

        return component;
    }
    //////////////////////////////////////////////////////////////////////////
}