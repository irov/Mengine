#include "BaseUpdation.h"

#include "Interface/UpdateServiceInterface.h"

#include "Kernel/Assertion.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BaseUpdation::BaseUpdation()
        : m_updatableProxyId( INVALID_UPDATABLE_ID )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    BaseUpdation::~BaseUpdation()
    {
        MENGINE_ASSERTION_FATAL( m_updatableProxyId == INVALID_UPDATABLE_ID );
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseUpdation::activate( EUpdateMode _mode, uint32_t _deep )
    {
        uint32_t updatableProxyId = UPDATE_SERVICE()
            ->createUpdatater( _mode, _deep, UpdationInterfacePtr::from( this ) );

        if( updatableProxyId == INVALID_UPDATABLE_ID )
        {
            return false;
        }

        m_updatableProxyId = updatableProxyId;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseUpdation::deactivate()
    {
        UPDATE_SERVICE()
            ->removeUpdatater( m_updatableProxyId );

        m_updatableProxyId = INVALID_UPDATABLE_ID;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseUpdation::replace( uint32_t _deep )
    {
        if( m_updatableProxyId == INVALID_UPDATABLE_ID )
        {
            return;
        }

        UPDATE_SERVICE()
            ->replaceUpdatater( m_updatableProxyId, _deep );
    }
    //////////////////////////////////////////////////////////////////////////
}