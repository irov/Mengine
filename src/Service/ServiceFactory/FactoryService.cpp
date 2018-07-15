#include "FactoryService.h"

#include "Kernel/Logger.h"

#include <algorithm>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( FactoryService, Mengine::FactoryService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FactoryService::FactoryService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FactoryService::~FactoryService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryService::registerFactory( const Factory * _factory )
    {
        m_factories.emplace_back( _factory );
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryService::unregisterFactory( const Factory * _factory )
    {
        TVectorFactories::iterator it_found = std::find( m_factories.begin(), m_factories.end(), _factory );

        if( it_found == m_factories.end() )
        {
            return;
        }

        m_factories.erase( it_found );
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryService::visitFactories( VisitorFactoryService * _visitor )
    {
        for( const Factory * factory : m_factories )
        {
            _visitor->visit( factory );
        }
    }
}