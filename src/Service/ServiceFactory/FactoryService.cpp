#include "FactoryService.h"

#include "Logger/Logger.h"

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
        for( TVectorFactories::const_iterator
            it = m_factories.begin(),
            it_end = m_factories.end();
            it != it_end;
            ++it )
        {
            const Factory * factory = *it;

            _visitor->visit( factory );
        }
    }
}