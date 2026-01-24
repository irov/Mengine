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
                return _deep * 2U + MENGINE_UINT32_C(0);
                break;
            case EUM_NODE_AFFECTOR:
                return _deep * 2U + MENGINE_UINT32_C(1);
                break;
            case EUM_SERVICE_BEFORE:
                return _deep;
                break;
            case EUM_SERVICE_AFTER:
                return _deep;
                break;
            default:
                return MENGINE_UINT32_C(~0);
            };
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    BaseUpdation::BaseUpdation()
        : m_mode( EUM_UNKNOWN )
        , m_deep( MENGINE_UINT32_C(~0) )
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

        UPDATE_SERVICE()
            ->placeUpdatater( this );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseUpdation::deactivate()
    {
        UPDATE_SERVICE()
            ->removeUpdatater( this );

        m_mode = EUM_UNKNOWN;
        m_deep = MENGINE_UINT32_C(~0);
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseUpdation::replace( uint32_t _deep )
    {
        uint32_t mode_deep = Detail::calcModeDeep( m_mode, _deep );

        if( m_deep == mode_deep )
        {
            return;
        }

        UPDATE_SERVICE()
            ->removeUpdatater( this );

        m_deep = mode_deep;

        UPDATE_SERVICE()
            ->placeUpdatater( this );
    }
    //////////////////////////////////////////////////////////////////////////
}