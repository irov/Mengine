#include "BaseUpdation.h"

#include "Interface/UpdateServiceInterface.h"

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
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseUpdation::activate( uint32_t _mode, uint32_t _deep )
    {
        m_updatableProxyId = UPDATE_SERVICE()
            ->createUpdatater( _mode, _deep, this );
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
    void BaseUpdation::update( const UpdateContext * _context )
    {
        this->_update( _context );
    }
}