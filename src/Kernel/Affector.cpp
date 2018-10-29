#include "Kernel/Affector.h"

#include "Interface/UpdationInterface.h"
#include "Interface/UpdateServiceInterface.h"
#include "Interface/TimelineServiceInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Affector::Affector()
        : m_type( ETA_POSITION )
        , m_id( 0 )
        , m_updataterId( INVALID_UPDATABLE_ID )
        , m_speedFactor( 1.f )
        , m_freeze( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Affector::~Affector()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Affector::setAffectorType( EAffectorType _type ) noexcept
    {
        m_type = _type;
    }
    //////////////////////////////////////////////////////////////////////////
    EAffectorType Affector::getAffectorType() const noexcept
    {
        return m_type;
    }
    //////////////////////////////////////////////////////////////////////////
    void Affector::setId( AFFECTOR_ID _id ) noexcept
    {
        m_id = _id;
    }
    //////////////////////////////////////////////////////////////////////////
    AFFECTOR_ID Affector::getId() const noexcept
    {
        return m_id;
    }
    //////////////////////////////////////////////////////////////////////////
    void Affector::setSpeedFactor( float _speedAffector ) noexcept
    {
        m_speedFactor = _speedAffector;
    }
    //////////////////////////////////////////////////////////////////////////
    float Affector::getSpeedFactor() const noexcept
    {
        return m_speedFactor;
    }
    //////////////////////////////////////////////////////////////////////////
    void Affector::setFreeze( bool _value ) noexcept
    {
        m_freeze = _value;

        this->_setFreeze( m_freeze );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Affector::getFreeze() const noexcept
    {
        return m_freeze;
    }
    //////////////////////////////////////////////////////////////////////////
    void Affector::_setFreeze( bool _value )
    {
        (void)_value;

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool Affector::prepare( uint32_t _updatableMode, uint32_t _updatableLeaf )
    {
        if( this->_prepare() == false )
        {
            return false;
        }

        UpdationInterfacePtr updation = this->getUpdation();

        m_updataterId = UPDATE_SERVICE()
            ->createUpdatater( _updatableMode, _updatableLeaf, updation );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Affector::_prepare()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Affector::_update( const UpdateContext * _context )
    {
        float used = 0.f;
        bool finish = this->_affect( _context, &used );

        if( finish == true )
        {
            stdex::intrusive_this_acquire( this );

            this->unlink();

            TIMELINE_SERVICE()
                ->beginOffset( used );

            this->complete( true );

            TIMELINE_SERVICE()
                ->endOffset();

            stdex::intrusive_this_release( this );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Affector::stop()
    {
        this->_stop();

        this->complete( false );
    }
    //////////////////////////////////////////////////////////////////////////
    void Affector::_stop()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Affector::complete( bool _isEnd )
    {
        (void)_isEnd;

        UPDATE_SERVICE()
            ->removeUpdatater( m_updataterId );

        m_updataterId = INVALID_UPDATABLE_ID;

        this->unlink();

        this->_complete( _isEnd );
    }
    //////////////////////////////////////////////////////////////////////////
    void Affector::_complete( bool _isEnd )
    {
        (void)_isEnd;
        //Empty
    }
}
