#include "UpdateService.h"

#include "Interface/TimepipeServiceInterface.h"

#include "Kernel/Updatable.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ProfilerHelper.h"
#include "Kernel/ConfigHelper.h"

#include "Config/Algorithm.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( UpdateService, Mengine::UpdateService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Detail
    {
        static uint32_t calcDeep( EUpdateMode _mode, uint32_t _deep )
        {
            switch( _mode )
            {
            case EUM_NODE_BASE:
                return _deep * 2U + 0U;
                break;
            case EUM_NODE_AFFECTOR:
                return _deep * 2U + 1U;
                break;
            case EUM_SERVICE_BEFORE:
                return _deep;
                break;
            case EUM_SERVICE_AFTER:
                return _deep;
                break;
            default:
                return ~0U;
            };
        }
    }
    //////////////////////////////////////////////////////////////////////////
    UpdateService::UpdateService()
        : m_timepipeId( INVALID_UNIQUE_ID )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    UpdateService::~UpdateService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool UpdateService::_initializeService()
    {
        uint32_t Engine_DefaultUpdateProxies = CONFIG_VALUE( "Engine", "DefaultUpdateProxies", 16384U );
        uint32_t Engine_DefaultUpdateLeafs = CONFIG_VALUE( "Engine", "DefaultUpdateLeafs", 256U );

        m_proxies.reserve( Engine_DefaultUpdateProxies );
        m_proxyFrees.reserve( Engine_DefaultUpdateProxies );

        m_beforeLeaf.resize( 16 );
        m_leafs.resize( Engine_DefaultUpdateLeafs );
        m_afterLeaf.resize( 16 );

        uint32_t timepipe = TIMEPIPE_SERVICE()
            ->addTimepipe( TimepipeInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE );

        m_timepipeId = timepipe;

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

        if( m_timepipeId != INVALID_UNIQUE_ID )
        {
            TIMEPIPE_SERVICE()
                ->removeTimepipe( m_timepipeId );

            m_timepipeId = INVALID_UNIQUE_ID;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void UpdateService::_stopService()
    {
        for( UpdatableProxy & proxy : m_proxies )
        {
            proxy.updation = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    UpdateService::LeafUpdatable * UpdateService::getLeafUpdatable( EUpdateMode _mode, uint32_t _deep )
    {
        switch( _mode )
        {
        case EUM_NODE_BASE:
            {
                MENGINE_ASSERTION_FATAL( _deep * 2 + 0 < m_leafs.size(), "deep %u"
                    , _deep
                );

                LeafUpdatable & leaf = m_leafs[_deep * 2 + 0];

                return &leaf;
            }break;
        case EUM_NODE_AFFECTOR:
            {
                MENGINE_ASSERTION_FATAL( _deep * 2 + 1 < m_leafs.size(), "deep %u"
                    , _deep
                );

                LeafUpdatable & leaf = m_leafs[_deep * 2 + 1];

                return &leaf;
            }break;
        case EUM_SERVICE_BEFORE:
            {
                MENGINE_ASSERTION_FATAL( _deep < m_beforeLeaf.size(), "deep %u"
                    , _deep
                );

                LeafUpdatable & leaf = m_beforeLeaf[_deep];

                return &leaf;
            }break;
        case EUM_SERVICE_AFTER:
            {
                MENGINE_ASSERTION_FATAL( _deep < m_afterLeaf.size(), "deep %u"
                    , _deep
                );

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
    uint32_t UpdateService::placeProxy_( EUpdateMode _mode, uint32_t _deep, const UpdationInterfacePtr & _updation )
    {
        UpdatableProxy proxy;
        proxy.updation = _updation;
        proxy.mode = _mode;
        proxy.deep = Detail::calcDeep( _mode, _deep );

        proxy.state = EUS_NORMAL;

        if( m_proxyFrees.empty() == true )
        {
            uint32_t new_id = (uint32_t)m_proxies.size();

            m_proxies.emplace_back( proxy );

            return new_id;
        }

        uint32_t free_id = m_proxyFrees.back();
        m_proxyFrees.pop_back();

        m_proxies[free_id] = proxy;

        return free_id;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t UpdateService::createUpdatater( EUpdateMode _mode, uint32_t _deep, const UpdationInterfacePtr & _updation )
    {
        uint32_t id = this->placeProxy_( _mode, _deep, _updation );

        LeafUpdatable * leaf = this->getLeafUpdatable( _mode, _deep );

        MENGINE_ASSERTION_MEMORY_PANIC( leaf, "unsupport mode '%u'"
            , _mode
        );

        leaf->indeciesAdd.emplace_back( id );

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    void UpdateService::replaceUpdatater( uint32_t _id, uint32_t _deep )
    {
        UpdatableProxy & proxy = m_proxies[_id];

        if( proxy.state == EUS_REMOVE )
        {
            return;
        }

        uint32_t mode_deep = Detail::calcDeep( proxy.mode, _deep );

        if( proxy.deep == mode_deep )
        {
            return;
        }

        proxy.deep = mode_deep;

        LeafUpdatable * new_leaf = this->getLeafUpdatable( proxy.mode, proxy.deep );

        new_leaf->indeciesAdd.emplace_back( _id );
    }
    //////////////////////////////////////////////////////////////////////////
    void UpdateService::removeUpdatater( uint32_t _id )
    {
        UpdatableProxy & proxy = m_proxies[_id];

        proxy.updation = nullptr;
        proxy.state = EUS_REMOVE;
    }
    //////////////////////////////////////////////////////////////////////////
    void UpdateService::updateLeaf_( uint32_t _deep, LeafUpdatable * const _leaf, const UpdateContext * _context )
    {
        if( _leaf->indeciesAdd.empty() == false )
        {
            _leaf->indecies.insert( _leaf->indecies.end(), _leaf->indeciesAdd.begin(), _leaf->indeciesAdd.end() );
            _leaf->indeciesAdd.clear();
        }

        for( VectorUpdatableIndecies::iterator
            it = _leaf->indecies.begin(),
            it_end = _leaf->indecies.end();
            it != it_end;)
        {
            uint32_t id = *it;

            UpdatableProxy & proxy = m_proxies[id];

            if( proxy.deep != _deep )
            {
                if( it + 1 == it_end )
                {
                    _leaf->indecies.pop_back();

                    break;
                }
                else
                {
                    *it = _leaf->indecies.back();
                    _leaf->indecies.pop_back();

                    it_end = _leaf->indecies.end();
                }

                continue;
            }
            else if( proxy.state == EUS_REMOVE )
            {
                m_proxyFrees.emplace_back( id );

                if( it + 1 == it_end )
                {
                    _leaf->indecies.pop_back();

                    break;
                }
                else
                {
                    *it = _leaf->indecies.back();
                    _leaf->indecies.pop_back();

                    it_end = _leaf->indecies.end();
                }

                continue;
            }

            const UpdationInterfacePtr & updation = proxy.updation;

            updation->update( _context );

            ++it;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void UpdateService::onTimepipe( const UpdateContext * _context )
    {
        MENGINE_PROFILER_CATEGORY();

        uint32_t enumerateBeforeDeep = 0U;
        for( LeafUpdatable & leaf : m_beforeLeaf )
        {
            this->updateLeaf_( enumerateBeforeDeep, &leaf, _context );

            ++enumerateBeforeDeep;
        }

        uint32_t enumerateDeep = 0U;
        for( LeafUpdatable & leaf : m_leafs )
        {
            this->updateLeaf_( enumerateDeep, &leaf, _context );

            ++enumerateDeep;
        }

        uint32_t enumerateAfterDeep = 0U;
        for( LeafUpdatable & leaf : m_afterLeaf )
        {
            this->updateLeaf_( enumerateAfterDeep, &leaf, _context );

            ++enumerateAfterDeep;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
