#include "ComponentService.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ComponentService, Mengine::ComponentService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ComponentService::ComponentService()
        : m_enumerator( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ComponentService::~ComponentService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ComponentService::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ComponentService::_finalizeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ComponentService::addComponentHub( const ConstString & _type, const ComponentHubInterfacePtr & _componentHub )
    {
        uint32_t id = m_enumerator++;
        uint32_t mask = 1 << id;

        _componentHub->setMask( mask );

        m_componentHubs.emplace( _type, _componentHub );
    }
    //////////////////////////////////////////////////////////////////////////
    ComponentHubInterfacePtr ComponentService::removeComponentHub( const ConstString & _type )
    {
        ComponentHubInterfacePtr componentHub = m_componentHubs.erase( _type );

        return componentHub;
    }
    //////////////////////////////////////////////////////////////////////////
    const ComponentHubInterfacePtr & ComponentService::getComponentHub( const ConstString & _type ) const
    {
        const ComponentHubInterfacePtr & componentHub = m_componentHubs.find( _type );

        return componentHub;
    }
    //////////////////////////////////////////////////////////////////////////
}
