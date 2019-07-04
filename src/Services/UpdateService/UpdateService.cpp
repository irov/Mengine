#include "UpdateService.h"

#include "Kernel/Updatable.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"

#include <algorithm>

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
    UpdateService::LeafUpdatable * UpdateService::getLeafUpdatable( uint32_t _mode, uint32_t _deep )
    {
        switch( _mode )
        {
        case 0:
            {
                LeafUpdatable & leaf = m_leafs[_deep * 2 + 0];

                return &leaf;
            }break;
        case 1:
            {
                LeafUpdatable & leaf = m_leafs[_deep * 2 + 1];

                return &leaf;
            }break;
        case 2:
            {
                LeafUpdatable & leaf = m_beforeLeaf[_deep];

                return &leaf;
            }break;
        case 3:
            {
                LeafUpdatable & leaf = m_afterLeaf[_deep];

                return &leaf;
            }break;
        default:
            {
            }break;
        }

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t UpdateService::placeProxy_( uint32_t _mode, uint32_t _deep, const UpdationInterfacePtr & _updation )
    {
        UpdatableProxy proxy;
        proxy.updation = _updation;
        proxy.mode = _mode;

        switch( proxy.mode )
        {
        case 0:
            proxy.deep = _deep * 2U + 0U;
            break;
        case 1:
            proxy.deep = _deep * 2U + 1U;
            break;
        default:
            proxy.deep = _deep;
            break;
        };

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
    uint32_t UpdateService::createUpdatater( uint32_t _mode, uint32_t _deep, const UpdationInterfacePtr & _updation )
    {
        uint32_t id = this->placeProxy_( _mode, _deep, _updation );

        LeafUpdatable * leaf = getLeafUpdatable( _mode, _deep );

        MENGINE_ASSERTION_MEMORY_PANIC( leaf, INVALID_UPDATABLE_ID, "unsupport mode '%d'"
            , _mode
        );

        leaf->indeciesAdd.push_back( id );

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    void UpdateService::replaceUpdatater( uint32_t _id, uint32_t _deep )
    {
        UpdatableProxy & proxy = m_proxies[_id];

        if( proxy.deep == _deep )
        {
            return;
        }

        if( proxy.state == 2 )
        {
            return;
        }

        LeafUpdatable * prev_leaf = getLeafUpdatable( proxy.mode, proxy.deep );

        VectorUpdatableIndecies::iterator it_found = std::find( prev_leaf->indeciesAdd.begin(), prev_leaf->indeciesAdd.end(), _id );

        if( it_found != prev_leaf->indeciesAdd.end() )
        {
            *it_found = prev_leaf->indeciesAdd.back();
            prev_leaf->indeciesAdd.pop_back();
        }

        switch( proxy.mode )
        {
        case 0:
            proxy.deep = _deep * 2U + 0U;
            break;
        case 1:
            proxy.deep = _deep * 2U + 1U;
            break;
        default:
            proxy.deep = _deep;
            break;
        };

        LeafUpdatable * new_leaf = getLeafUpdatable( proxy.mode, proxy.deep );

        new_leaf->indeciesAdd.push_back( _id );
    }
    //////////////////////////////////////////////////////////////////////////
    void UpdateService::removeUpdatater( uint32_t _id )
    {
        UpdatableProxy & proxy = m_proxies[_id];

        proxy.updation = nullptr;
        proxy.state = 2;
    }
    //////////////////////////////////////////////////////////////////////////
    void UpdateService::updateLeaf_( uint32_t _deep, LeafUpdatable & _leaf, const UpdateContext * _context )
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

            if( proxy.deep != _deep )
            {
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
            else if( proxy.state == 1 )
            {
                ++it;

                continue;
            }
            else if( proxy.state == 2 )
            {
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
        uint32_t enumerateBeforeDeep = 0U;
        for( LeafUpdatable & leaf : m_beforeLeaf )
        {
            this->updateLeaf_( enumerateBeforeDeep, leaf, _context );

            ++enumerateBeforeDeep;
        }

        uint32_t enumerateDeep = 0U;
        for( LeafUpdatable & leaf : m_leafs )
        {
            this->updateLeaf_( enumerateDeep, leaf, _context );

            ++enumerateDeep;
        }

        uint32_t enumerateAfterDeep = 0U;
        for( LeafUpdatable & leaf : m_afterLeaf )
        {
            this->updateLeaf_( enumerateAfterDeep, leaf, _context );

            ++enumerateAfterDeep;
        }
    }
}
