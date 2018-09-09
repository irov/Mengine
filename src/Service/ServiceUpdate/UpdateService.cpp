#include "UpdateService.h"

#include "Kernel/Updatable.h"
#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( UpdateService, Mengine::UpdateService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    UpdateService::UpdateService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    UpdateService::~UpdateService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool UpdateService::_initializeService()
    {
        m_proxies.reserve( 16384 );
        m_proxyFrees.reserve( 16384 );

        m_beforeLeaf.resize( 16 );
        m_leafs.resize( 256 );
        m_afterLeaf.resize( 16 );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void UpdateService::_finalizeService()
    {
        m_beforeLeaf.clear();
        m_leafs.clear();
        m_afterLeaf.clear();

        m_proxyFrees.clear();
        m_proxies.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t UpdateService::placeProxy_( const UpdationInterfacePtr & _updation )
    {
        UpdatableProxy proxy;
        proxy.updation = _updation;
        proxy.state = 0;

        if( m_proxyFrees.empty() == true )
        {
            uint32_t new_id = (uint32_t)m_proxies.size();

            m_proxies.push_back( proxy );

            return new_id;
        }

        uint32_t free_id = m_proxyFrees.back();
        m_proxyFrees.pop_back();

        m_proxies[free_id] = proxy;

        return free_id;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t UpdateService::createUpdatater( uint32_t _mode, uint32_t _leaf, const UpdationInterfacePtr & _updation )
    {
        uint32_t id = this->placeProxy_( _updation );
        
        switch( _mode )
        {
        case 0:
            {
                LeafUpdatable & leaf = m_leafs[_leaf * 2 + 0];
                leaf.indeciesAdd.push_back( id );
            }break;
        case 1:
            {
                LeafUpdatable & leaf = m_leafs[_leaf * 2 + 1];
                leaf.indeciesAdd.push_back( id );
            }break;
        case 2:
            {
                LeafUpdatable & leaf = m_beforeLeaf[_leaf];
                leaf.indeciesAdd.push_back( id );
            }break;
        case 3:
            {
                LeafUpdatable & leaf = m_afterLeaf[_leaf];
                leaf.indeciesAdd.push_back( id );
            }break;
        default:
            {
                LOGGER_ERROR( "UpdateService::createUpdatater unsupport mode '%d'"
                    , _mode
                );

                return INVALID_UPDATABLE_ID;
            }break;
        }

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    void UpdateService::removeUpdatater( uint32_t _id )
    {
        UpdatableProxy & proxy = m_proxies[_id];

        proxy.state = 3;
    }
    //////////////////////////////////////////////////////////////////////////
    void UpdateService::updateLeaf_( LeafUpdatable & _leaf, const UpdateContext * _context )
    {
        _leaf.indecies.insert( _leaf.indecies.end(), _leaf.indeciesAdd.begin(), _leaf.indeciesAdd.end() );
        _leaf.indeciesAdd.clear();

        for( VectorUpdatableIndecies::iterator
            it = _leaf.indecies.begin(),
            it_end = _leaf.indecies.end();
            it != it_end;)
        {
            uint32_t id = *it;

            UpdatableProxy & proxy = m_proxies[id];

            if( proxy.state == 1 )
            {
                ++it;

                continue;
            }
            else if( proxy.state == 3 )
            {
                proxy.updation = nullptr;

                m_proxyFrees.push_back( id );

                if( it + 1 == it_end )
                {
                    _leaf.indecies.pop_back();
                    break;
                }
                else
                {
                    *it = _leaf.indecies.back();
                    _leaf.indecies.pop_back();

                    it_end = _leaf.indecies.end();
                }

                continue;
            }

            const UpdationInterfacePtr & updation = proxy.updation;

            updation->update( _context );

            ++it;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void UpdateService::update( const UpdateContext * _context )
    {
        for( LeafUpdatable & leaf : m_beforeLeaf )
        {
            this->updateLeaf_( leaf, _context );
        }

        for( LeafUpdatable & leaf : m_leafs )
        {
            this->updateLeaf_( leaf, _context );
        }

        for( LeafUpdatable & leaf : m_afterLeaf )
        {
            this->updateLeaf_( leaf, _context );
        }
    }
}
