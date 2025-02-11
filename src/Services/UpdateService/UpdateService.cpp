#include "UpdateService.h"

#include "Interface/TimepipeServiceInterface.h"

#include "Kernel/Updatable.h"
#include "Kernel/Logger.h"
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
        uint32_t Engine_DefaultUpdateLeafs = CONFIG_VALUE_INTEGER( "Engine", "DefaultUpdateLeafs", 256U );

        m_beforeLeaf.resize( 32 );
        m_leafs.resize( Engine_DefaultUpdateLeafs );
        m_afterLeaf.resize( 32 );

        TIMEPIPE_SERVICE()
            ->addTimepipe( TimepipeInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void UpdateService::_finalizeService()
    {
        TIMEPIPE_SERVICE()
            ->removeTimepipe( TimepipeInterfacePtr::from( this ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void UpdateService::_stopService()
    {
        m_beforeLeaf.clear();
        m_leafs.clear();
        m_afterLeaf.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    UpdateService::LeafUpdatable * UpdateService::getLeafUpdatable( EUpdateMode _mode, uint32_t _deep )
    {
        switch( _mode )
        {
        case EUM_NODE_BASE:
        case EUM_NODE_AFFECTOR:
            {
                MENGINE_ASSERTION_FATAL( _deep < m_leafs.size(), "deep %u"
                    , _deep
                );

                LeafUpdatable & leaf = m_leafs[_deep];

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
    void UpdateService::placeUpdatater( const UpdationInterfacePtr & _updation )
    {
        EUpdateMode update_mode = _updation->getMode();
        uint32_t update_deep = _updation->getDeep();

        LeafUpdatable * leaf = this->getLeafUpdatable( update_mode, update_deep );

        MENGINE_ASSERTION_MEMORY_PANIC( leaf, "unsupport mode '%u' deep '%u'"
            , update_mode
            , update_deep
        );

        leaf->indeciesAdd.emplace_back( _updation );
    }
    //////////////////////////////////////////////////////////////////////////
    void UpdateService::updateLeaf_( uint32_t _deep, LeafUpdatable * const _leaf, const UpdateContext * _context )
    {
        MENGINE_UNUSED( _deep );

        VectorUpdatableIndecies & leaf_indecies = _leaf->indecies;
        VectorUpdatableIndecies & leaf_indeciesAdd = _leaf->indeciesAdd;

        if( leaf_indeciesAdd.empty() == false )
        {
            leaf_indecies.insert( leaf_indecies.end(), leaf_indeciesAdd.begin(), leaf_indeciesAdd.end() );
            leaf_indeciesAdd.clear();
        }

        leaf_indecies.erase( Algorithm::remove_if( leaf_indecies.begin(), leaf_indecies.end(), [_deep]( const UpdationInterfacePtr & _updation )
        {
            uint32_t updation_deep = _updation->getDeep();

            if( updation_deep != _deep )
            {
                return true;
            }

            EUpdateState updation_state = _updation->getState();

            if( updation_state == EUS_REMOVE )
            {
                return true;
            }

            return false;
        } ), leaf_indecies.end() );

        for( const UpdationInterfacePtr & updation : leaf_indecies )
        {
            uint32_t updation_deep = updation->getDeep();

            if( updation_deep != _deep )
            {
                continue;
            }

            EUpdateState updation_state = updation->getState();

            if( updation_state == EUS_REMOVE )
            {
                continue;
            }

            updation->update( _context );
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
