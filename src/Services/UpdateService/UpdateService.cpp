#include "UpdateService.h"

#include "Interface/TimepipeServiceInterface.h"

#include "Kernel/Updatable.h"
#include "Kernel/Logger.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ProfilerHelper.h"
#include "Kernel/ConfigHelper.h"

#include "Config/StdAlgorithm.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( UpdateService, Mengine::UpdateService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        bool findLeafUpdatater( const LeafUpdatables & _leafs, const UpdationInterface * _updation )
        {
            for( const LeafUpdatable & leaf : _leafs )
            {
                if( std::find( leaf.indecies.begin(), leaf.indecies.end(), _updation ) != leaf.indecies.end() )
                {
                    return true;
                }

                if( std::find( leaf.indeciesAdd.begin(), leaf.indeciesAdd.end(), _updation ) != leaf.indeciesAdd.end() )
                {
                    return true;
                }
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
    }
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

        m_beforeLeafs.resize( 32 );
        m_leafs.resize( Engine_DefaultUpdateLeafs );
        m_afterLeafs.resize( 32 );

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
        m_beforeLeafs.clear();
        m_leafs.clear();
        m_afterLeafs.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    LeafUpdatable * UpdateService::getLeafUpdatable( EUpdateMode _mode, uint32_t _deep )
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
                MENGINE_ASSERTION_FATAL( _deep < m_beforeLeafs.size(), "deep %u"
                    , _deep
                );

                LeafUpdatable & leaf = m_beforeLeafs[_deep];

                return &leaf;
            }break;
        case EUM_SERVICE_AFTER:
            {
                MENGINE_ASSERTION_FATAL( _deep < m_afterLeafs.size(), "deep %u"
                    , _deep
                );

                LeafUpdatable & leaf = m_afterLeafs[_deep];

                return &leaf;
            }break;
        default:
            {
            }break;
        }

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void UpdateService::placeUpdatater( UpdationInterface * _updation )
    {
        MENGINE_ASSERTION_FATAL( this->findUpdatater_( _updation ) == false, "updation already exist" );

        EUpdateMode update_mode = _updation->getUpdationMode();
        uint32_t update_deep = _updation->getUpdationDeep();

        MENGINE_ASSERTION_FATAL( update_mode != EUM_UNKNOWN, "updation mode is UNKNOWN" );
        MENGINE_ASSERTION_FATAL( update_deep != ~0U, "updation deep is INVALID" );

        LeafUpdatable * leaf = this->getLeafUpdatable( update_mode, update_deep );

        MENGINE_ASSERTION_MEMORY_PANIC( leaf, "unsupport mode '%u' deep '%u'"
            , update_mode
            , update_deep
        );

        leaf->indeciesAdd.emplace_back( _updation );
    }
    //////////////////////////////////////////////////////////////////////////
    void UpdateService::removeUpdatater( UpdationInterface * _updation )
    {
        MENGINE_ASSERTION_FATAL( this->findUpdatater_( _updation ) == true, "updation already exist" );

        EUpdateMode update_mode = _updation->getUpdationMode();
        uint32_t update_deep = _updation->getUpdationDeep();

        MENGINE_ASSERTION_FATAL( update_mode != EUM_UNKNOWN, "updation mode is UNKNOWN" );
        MENGINE_ASSERTION_FATAL( update_deep != ~0U, "updation deep is INVALID" );

        LeafUpdatable * leaf = this->getLeafUpdatable( update_mode, update_deep );

        MENGINE_ASSERTION_MEMORY_PANIC( leaf, "unsupport mode '%u' deep '%u'"
            , update_mode
            , update_deep
        );

        VectorUpdatableIndecies::iterator it_indecies_found = std::find( leaf->indecies.begin(), leaf->indecies.end(), _updation );

        if( it_indecies_found != leaf->indecies.end() )
        {
            *it_indecies_found = nullptr;

            return;
        }

        VectorUpdatableIndecies::iterator it_indecies_add_found = std::find( leaf->indeciesAdd.begin(), leaf->indeciesAdd.end(), _updation );

        if( it_indecies_add_found != leaf->indeciesAdd.end() )
        {
            *it_indecies_add_found = nullptr;

            return;
        }

        MENGINE_ASSERTION_FATAL( false, "updation not found mode '%u' deep '%u'"
            , update_mode
            , update_deep
        );
    }
    //////////////////////////////////////////////////////////////////////////
    bool UpdateService::findUpdatater_( const UpdationInterface * _updation ) const
    {
        if( Detail::findLeafUpdatater( m_beforeLeafs, _updation ) == true )
        {
            return true;
        }

        if( Detail::findLeafUpdatater( m_leafs, _updation ) == true )
        {
            return true;
        }

        if( Detail::findLeafUpdatater( m_afterLeafs, _updation ) == true )
        {
            return true;
        }

        return false;
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

        leaf_indecies.erase( StdAlgorithm::remove( leaf_indecies.begin(), leaf_indecies.end(), nullptr ), leaf_indecies.end() );

        for( UpdationInterface * updation : leaf_indecies )
        {
            if( updation == nullptr )
            {
                continue;
            }

            uint32_t updation_deep = updation->getUpdationDeep();

            if( updation_deep != _deep )
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
        for( LeafUpdatable & leaf : m_beforeLeafs )
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
        for( LeafUpdatable & leaf : m_afterLeafs )
        {
            this->updateLeaf_( enumerateAfterDeep, &leaf, _context );

            ++enumerateAfterDeep;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
