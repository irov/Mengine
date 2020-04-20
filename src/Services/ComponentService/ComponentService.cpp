#include "ComponentService.h"

#include "Kernel/Assertion.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ComponentService, Mengine::ComponentService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ComponentService::ComponentService()
        : m_enumerator( 0U )
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
        m_componentIds.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ComponentService::makeComponentId( const std::type_index & _index )
    {
        MapComponentIds::iterator it_found = m_componentIds.find( _index );

        if( it_found != m_componentIds.end() )
        {
            uint32_t id = it_found->second;

            return id;
        }

        MENGINE_ASSERTION_FATAL( m_enumerator < 32 );

        uint32_t new_id = m_enumerator++;

        m_componentIds.emplace_hint( it_found, _index, new_id );

        return new_id;
    }
}