#include "Kernel/Affector.h"

#include "Interface/UpdationInterface.h"
#include "Interface/UpdateServiceInterface.h"
#include "Interface/TimelineServiceInterface.h"

#include "Kernel/IntrusivePtrScope.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Affector::Affector()
        : m_type( EAFFECTORTYPE_POSITION )
        , m_status( EAFFECTORSTATUS_IDLE )
        , m_id( INVALID_UNIQUE_ID )
        , m_speedFactor( 1.f )
        , m_freeze( false )
        , m_affecting( false )
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
    void Affector::setId( UniqueId _id ) noexcept
    {
        m_id = _id;
    }
    //////////////////////////////////////////////////////////////////////////
    UniqueId Affector::getId() const noexcept
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
        MENGINE_UNUSED( _value );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Affector::setEasing( const EasingInterfacePtr & _easing )
    {
        m_easing = _easing;
    }
    //////////////////////////////////////////////////////////////////////////
    const EasingInterfacePtr & Affector::getEasing() const
    {
        return m_easing;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Affector::prepare( EUpdateMode _updatableMode, uint32_t _updatableLeaf )
    {
        m_status = EAFFECTORSTATUS_PREPARE;

        if( this->_prepare() == false )
        {
            return false;
        }

        UpdationInterface * updation = this->getUpdation();

        updation->activate( _updatableMode, _updatableLeaf );        

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Affector::_prepare()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Affector::update( const UpdateContext * _context )
    {
        float used = 0.f;

        m_affecting = true;
        bool finish = this->_affect( _context, &used );
        m_affecting = false;

        if( finish == true )
        {
            IntrusivePtrScope ankh( this );

            this->IntrusiveSlugLinkedPtr<Affector, void, IntrusivePtr, IntrusivePtrBase>::unlink();

            TIMELINE_SERVICE()
                ->beginOffset( used, MENGINE_DOCUMENT_FACTORABLE );

            this->complete( true );

            TIMELINE_SERVICE()
                ->endOffset();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Affector::stop()
    {
        m_status = EAFFECTORSTATUS_STOP;

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
        MENGINE_UNUSED( _isEnd );

        MENGINE_ASSERTION_FATAL( m_affecting == false, "affector '%u' can not completed or removing in _affect (please return true if you want complete)"
            , this->getId()
        );

        m_status = EAFFECTORSTATUS_COMPLETE;

        UpdationInterface * updation = this->getUpdation();

        updation->deactivate();

        this->IntrusiveSlugLinkedPtr<Affector, void, IntrusivePtr, IntrusivePtrBase>::unlink();

        this->_complete( _isEnd );
    }
    //////////////////////////////////////////////////////////////////////////
    void Affector::_complete( bool _isEnd )
    {
        MENGINE_UNUSED( _isEnd );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}
