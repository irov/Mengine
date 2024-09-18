#include "BaseUpdation.h"

#include "Interface/UpdateServiceInterface.h"

#include "Kernel/Assertion.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static uint32_t calcModeDeep( EUpdateMode _mode, uint32_t _deep )
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
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    BaseUpdation::BaseUpdation()
        : m_mode( EUM_NODE_BASE )
        , m_state( EUS_NORMAL )
        , m_deep( 0U )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    BaseUpdation::~BaseUpdation()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseUpdation::activate( EUpdateMode _mode, uint32_t _deep )
    {
        m_mode = _mode;

        uint32_t mode_deep = Detail::calcModeDeep( m_mode, _deep );
        m_deep = mode_deep;

        m_state = EUS_NORMAL;

        UPDATE_SERVICE()
            ->placeUpdatater( UpdationInterfacePtr::from( this ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseUpdation::deactivate()
    {
        m_state = EUS_REMOVE;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseUpdation::replace( uint32_t _deep )
    {
        if( m_state == EUS_REMOVE )
        {
            return;
        }

        uint32_t mode_deep = Detail::calcModeDeep( m_mode, _deep );

        if( m_deep == mode_deep )
        {
            return;
        }

        m_deep = mode_deep;

        UPDATE_SERVICE()
            ->placeUpdatater( UpdationInterfacePtr::from( this ) );
    }
    //////////////////////////////////////////////////////////////////////////
    EUpdateMode BaseUpdation::getMode() const
    {
        return m_mode;
    }
    //////////////////////////////////////////////////////////////////////////
    EUpdateState BaseUpdation::getState() const
    {
        return m_state;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t BaseUpdation::getDeep() const
    {
        return m_deep;
    }
    //////////////////////////////////////////////////////////////////////////
}